from __future__ import division

import json
import math
import traceback
import time
import Queue
import scipy

import numpy
from std_msgs.msg import String
from twisted.internet import defer
import sensor_msgs.point_cloud2 as pc2

from txros import action, util, tf

import rospy

#SPP having access to the gps methods will allow us to easily switch from ECEF to lat/long
import rawgps_common

import genpy
from std_msgs.msg import Header, Float64, Int16, Bool
from uf_common.msg import MoveToAction, MoveToGoal, PoseTwistStamped, Float64Stamped
from legacy_vision.msg import FindAction, FindGoal
from uf_common import orientation_helpers
from tf import transformations
from legacy_vision import msg as legacy_vision_msg
from object_finder import msg as object_finder_msg
from nav_msgs.msg import Odometry
from rawgps_common import gps
#SPP for hydropones
from hydrophones.msg import ProcessedPing
#SPP for getting gps fix lat/long
from geometry_msgs.msg import PointStamped, Wrench, WrenchStamped, Vector3
#for dynamixel configs
from dynamixel_servo.msg import DynamixelFullConfig 
from rise_6dof.srv import SendConstantWrench, SendConstantWrenchRequest
from sensor_msgs.msg import LaserScan, PointCloud2
from sensor_msgs.msg import Image
from lidar_vision.srv import lidar_servo_mode, lidar_servo_modeRequest
from lidar_vision.srv import lidar_servo_mode, lidar_servo_modeRequest
from azi_drive.srv import trajectory_mode, trajectory_modeRequest
from camera_docking.msg import Circle, Triangle, Cross
from azi_drive.srv import AziFloat, AziFloatRequest
import object_handling.msg
import vision_sandbox.msg
                                      
            
class _PoseProxy(object):
    def __init__(self, boat, pose):
        self._boat = boat
        self._pose = pose
    
    def __getattr__(self, name):
        def _(*args, **kwargs):
            return _PoseProxy(self._boat, getattr(self._pose, name)(*args, **kwargs))
        return _
    
    def go(self, *args, **kwargs):
        return self._boat._moveto_action_client.send_goal(
            self._pose.as_MoveToGoal(*args, **kwargs)).get_result()

class _Boat(object):
    def __init__(self, node_handle):
        self._node_handle = node_handle
    
    @util.cancellableInlineCallbacks
    def _init(self, need_trajectory=True, need_odom=True):
        self._trajectory_sub = self._node_handle.subscribe('trajectory', PoseTwistStamped)
        self._moveto_action_client = action.ActionClient(self._node_handle, 'moveto', MoveToAction)
        self._tf_listener = tf.TransformListener(self._node_handle)
        self._camera_2d_action_clients = dict(
            forward=action.ActionClient(self._node_handle, 'find2_forward_camera', legacy_vision_msg.FindAction),
        )
        self._camera_3d_action_clients = dict(
            forward=action.ActionClient(self._node_handle, 'find_forward', object_finder_msg.FindAction),
        )
        self._odom_sub = self._node_handle.subscribe('odom', Odometry)
        self._wrench_sub = self._node_handle.subscribe('wrench', WrenchStamped)
        #The absodom topic has the lat long information ebeded in the nav_msgs/Odometry. the lat long is under pose.pose.position and this is what is reported back in the JSON messages for a chalenge.
        self._absodom_sub = self._node_handle.subscribe('absodom', Odometry)
        #SPP subscribe the boat to the processed pings, so the missions can utalize the processed acoustic pings
        self._hydrophone_ping_sub = self._node_handle.subscribe('hydrophones/processed', ProcessedPing)
        #SPP subscribe the boat to the GPS messages, so the missions can easly get the data for their JSON messages
        self._hydrophone_freq_sub= self._node_handle.subscribe('hydrophones/desired_freq', Float64)
        
        self.servo_full_config_pub = self._node_handle.advertise('dynamixel/dynamixel_full_config', DynamixelFullConfig)
        
        self._send_constant_wrench_service = self._node_handle.get_service_client('send_constant_wrench', SendConstantWrench)

        self._set_lidar_mode = self._node_handle.get_service_client('lidar/lidar_servo_mode', lidar_servo_mode)

        self._set_path_planner_mode = self._node_handle.get_service_client('/azi_waypoint_mode', trajectory_mode)

        self._lidar_sub_raw = self._node_handle.subscribe('lidar/scan', LaserScan)

        self._lidar_sub_pointcloud = self._node_handle.subscribe('lidar/raw_pc', PointCloud2)
        
        self._buoy_sub = self._node_handle.subscribe('/object_handling/buoys', object_handling.msg.Buoys)
        self._gate_sub = self._node_handle.subscribe('/object_handling/gates', object_handling.msg.Gates)

        self._start_gate_vision_sub = self._node_handle.subscribe('start_gate_vision', Float64)

        self._circle_sub = self._node_handle.subscribe("Circle_Sign" , Circle)
        self._cross_sub = self._node_handle.subscribe("Cross_Sign" , Cross)
        self._triangle_sub = self._node_handle.subscribe("Triangle_Sign" , Triangle)

        self._odom_pub = self._node_handle.advertise('odom', Odometry)
    
        self.float_srv = self._node_handle.get_service_client('/float_mode', AziFloat)

        self_bouy_subsriber = self._node_handle.subscribe('vision_sandbox/buoy_info', vision_sandbox.msg.Buoys)

        
        # Make sure trajectory topic is publishing 
        if(need_trajectory == True):
            print 'Boat class __init__: Waiting on trajectory..'
            yield self._trajectory_sub.get_next_message()
            print 'Boat class __init__: Got trajectory'


        # Make sure odom is publishing
        if(need_odom == True):
            print 'Boat class __init__: Waiting on odom...'
            yield self._odom_sub.get_next_message()
            print 'Boat class __init__: Got odom'
        
        defer.returnValue(self)

    # Do not use Pose unless you are sure that trajectory is being published (i.e. 
    #    when you got a boat need_trajectory was set to true)
    @property
    def pose(self):
        return orientation_helpers.PoseEditor.from_PoseTwistStamped(
            self._trajectory_sub.get_last_message())
    @property
    def odom(self):
        return orientation_helpers.PoseEditor.from_Odometry(
            self._odom_sub.get_last_message())
    
    @property
    def move(self):
        return _PoseProxy(self, self.pose)
        
    @util.cancellableInlineCallbacks
    def get_bouys(self):
        msg = yield self._buoy_sub.get_next_message()
        defer.returnValue(msg)

    @util.cancellableInlineCallbacks
    def get_bouy_color(self):
        msg = yield self._buoy_sub.get_next_message()
        defer.returnValue(msg[0].buoys.color)


    def pan_lidar(self, freq = 0.5, min_angle = 2.7, max_angle = 3.4):
        self._set_lidar_mode(lidar_servo_modeRequest(
                    mode = lidar_servo_modeRequest.PAN,
                    freq = freq,
                    min_angle = min_angle,
                    max_angle = max_angle))

    

    def still_lidar(self, nominal_angle = numpy.pi):
        self._set_lidar_mode(lidar_servo_modeRequest(
                    mode = lidar_servo_modeRequest.STATIC,
                    nominal_angle = nominal_angle))

    def switch_path_planner(self, mode):
        self._set_path_planner_mode(trajectory_modeRequest(mode = mode))
   
    def float_on(self):
        response = self.float_srv(AziFloatRequest(True))    

    def float_off(self):
        response = self.float_srv(AziFloatRequest(False))
    
    @util.cancellableInlineCallbacks
    def wait_for_bump(self):
        while True:
            wrench = yield self._wrench_sub.get_next_message()
            if wrench.wrench.force.x > 60:
                break
    
    @util.cancellableInlineCallbacks
    def deploy_hydrophone(self):
        #INITIALLY WHEN THE BOAT STARTS THE CABLE SHOULD BE FEED OVER THE TOP OF THE SERVO
        deploy_msg=DynamixelFullConfig()
        deploy_msg.id=4 #id 4 is the stern servo for the hydrophones
        deploy_msg.led=0
        deploy_msg.goal_position=-2*math.pi
        deploy_msg.moving_speed=1.4 # 1.4 rad/s~22 rpm
        deploy_msg.torque_limit=173 # 173/1023 is about 17% torque
        deploy_msg.goal_acceleration=20
        deploy_msg.control_mode=DynamixelFullConfig.CONTINUOUS_ANGLE
        deploy_msg.goal_velocity=1.4
        for i in xrange(100):
            self.servo_full_config_pub.publish(deploy_msg)
            yield util.sleep(8.5/100)
    
    @util.cancellableInlineCallbacks
    def retract_hydrophone(self):
        #WHEN THE HYDROPHONES RETRACT CABLE SHOULD FEED BACK OVER THE TOP OF THE SERVO
        deploy_msg=DynamixelFullConfig()
        deploy_msg.id=4 #id 4 is the stern servo for the hydrophones
        deploy_msg.led=0
        deploy_msg.goal_position=4.3 # 2.4 rad/s~22 rpm NOTE: we explicitly retract to pi to try and avoid being at the 0/2*PI boundary on a powerup
        deploy_msg.moving_speed=1.4 # 1.4 rad/s~22 rpm
        deploy_msg.torque_limit= 205 # 205/1023 is about 20% torque (so we don't break the rope if someone didn't feed them correctly to start)
        deploy_msg.goal_acceleration=20
        deploy_msg.control_mode=DynamixelFullConfig.CONTINUOUS_ANGLE
        deploy_msg.goal_velocity=1.6
        self.servo_full_config_pub.publish(deploy_msg)
        for i in xrange(100):
            self.servo_full_config_pub.publish(deploy_msg)
            yield util.sleep(20/100)

    @util.cancellableInlineCallbacks
    def get_distance_from_object(self, radius):

        temp_distance = 0
        avg_distance = 0
        shortest_distance = 100
        farthest_distance = 0
        return_array = []
        hold = []

        while len(hold) <= 0:
            # get pointcloud
            pointcloud = yield self.get_pointcloud()
            yield util.sleep(.2) # sleep to avoid tooPast errors
            pointcloud_base = yield self.to_baselink(pointcloud)
            yield util.sleep(.2) # sleep to avoid tooPast errors

            # Filter lidar data to only data right in front of the boat
            hold = filter(lambda x: abs(x[1]) < radius, pointcloud_base)

        # Calculate several distances between target and boat
        for x in range(len(hold)):
            dist = hold[x]
            temp_distance += dist[0]
            # Check and assign the closest object to the boat
            if dist[0] < shortest_distance: shortest_distance = dist[0]
            if dist[0] > farthest_distance: farthest_distance = dist[0]

        avg_distance = temp_distance/len(hold)
        shortest_distance = shortest_distance
        farthest_distance = farthest_distance
        return_array.append(avg_distance)
        return_array.append(shortest_distance)
        return_array.append(farthest_distance)
        defer.returnValue(return_array)
      
    @util.cancellableInlineCallbacks
    def get_hydrophone_freq(self):
        msg = yield self._hydrophone_freq_sub.get_next_message()
        defer.returnValue(msg)
        
    #SPP get the latest processed acoustic message that is at the specified frequency
    @util.cancellableInlineCallbacks
    def get_processed_ping(self, (min_freq, max_freq)):
        while True:
            # keep looking for a ping at the specified frequency
            msg = yield self._hydrophone_ping_sub.get_next_message()
            if msg.freq >= min_freq and msg.freq <= max_freq:
                # only if you receive one should you return. NOTE: mission_core run_missions will timeout and kill this task so it wont run forever()
                defer.returnValue(msg)     

    @util.cancellableInlineCallbacks
    def hold_at_current_pos(self):
        move = self.move.set_position(self.odom.position).go()
        yield util.sleep(3)
        move.cancel()

              
    @util.cancellableInlineCallbacks
    def get_shape_location(self, shape):

        ret = []
        if shape == 'circle':
            msg = yield self._circle_sub.get_next_message()
            ret.append(msg.xpixel)
            ret.append(msg.color)
            defer.returnValue(ret)
        if shape == 'cruciform':
            msg = yield self._cross_sub.get_next_message()
            ret.append(msg.xpixel)
            ret.append(msg.color)
            defer.returnValue(ret)
        if shape == 'triangle':
            msg = yield self._triangle_sub.get_next_message()
            ret.append(msg.xpixel)
            ret.append(msg.color)
            defer.returnValue(ret)

        print 'Invalid shape ', shape
        assert False
    
    #SPP get the latest gps lat/long fix from the 
    @util.cancellableInlineCallbacks
    def get_gps_lat_long(self):
        msg = yield self._absodom_sub.get_next_message()
        #lat long is under msg.pose.pose.position of the nav_msgs/Odometry for the '/absodom' topic
        # Note: /absodom is Earth-Centered,Earth-Fixed (ECEF), so This means that ECEF rotates with the earth and a point fixed on the surface of the earth do not change.
        # See: http://en.wikipedia.org/wiki/ECEF
        temp=latlongheight_from_ecef(msg.pose.pose.position.x,msg.pose.pose.position.y,msg.pose.pose.position.z)
        ret_dict={'latitude' : temp[0],'longitude':temp[1]}
        defer.returnValue(ret_dict)

    @util.cancellableInlineCallbacks
    def to_baselink(self, msg):
        transform = yield self._tf_listener.get_transform('/base_link', msg.header.frame_id, msg.header.stamp)
        res = []
        for p in pc2.read_points(msg, field_names=("x", "y", "z"), skip_nans=False, uvs=[]):
            res.append(transform.transform_point((p[0], p[1], p[2])))
        defer.returnValue(res)

    @util.cancellableInlineCallbacks
    def to_enu(self, msg):
        res = []
        for p in pc2.read_points(msg, field_names=("x", "y", "z"), skip_nans=False, uvs=[]):
            res.append((p[0], p[1], p[2]))
        defer.returnValue(res)
    
    @util.cancellableInlineCallbacks
    def get_gps_odom(self):
        msg = yield self._absodom_sub.get_next_message()
        defer.returnValue(msg)

    @util.cancellableInlineCallbacks
    def get_gps_odom_rel(self):
        msg = yield self._odom_sub.get_next_message()
        defer.returnValue(msg)
    
    @util.cancellableInlineCallbacks
    def get_scan(self):
        msg = yield self._lidar_sub_raw.get_next_message()
        defer.returnValue(msg)

    @util.cancellableInlineCallbacks
    def get_pointcloud(self):
        msg = yield self._lidar_sub_pointcloud.get_next_message()
        defer.returnValue(msg)
    
    @util.cancellableInlineCallbacks
    def get_buoys(self):
        msg = yield self._buoy_sub.get_next_message()
        defer.returnValue(msg.buoys)

    @util.cancellableInlineCallbacks
    def get_gates(self):
        msg = yield self._gate_sub.get_next_message()
        defer.returnValue(msg.gates)
        
    @util.cancellableInlineCallbacks
    def get_start_gate_vision(self):
        msg = yield self._start_gate_vision_sub.get_next_message()
        defer.returnValue(msg.data)
    
    @util.cancellableInlineCallbacks
    def get_ecef_pos(self):
        msg = yield self._absodom_sub.get_next_message()
        defer.returnValue(orientation_helpers.xyz_array(msg.pose.pose.position))
        
    # Enter a default state
    #   retract_hydrophone
    #   default pan_lidar
    #   float off
    #   point_shoot_2_pp
    def default_state(self):
        self.retract_hydrophone()
        self.hold_at_current_pos()
        self.pan_lidar()
        self.float_off()
        self.switch_path_planner('point_shoot_2_pp')

_boats = {}
@util.cancellableInlineCallbacks
def get_boat(node_handle, need_trajectory=True, need_odom=True):
    if node_handle not in _boats:
        _boats[node_handle] = None # placeholder to prevent this from happening reentrantly
        _boats[node_handle] = yield _Boat(node_handle)._init(need_trajectory, need_odom)
        # XXX remove on nodehandle shutdown
    defer.returnValue(_boats[node_handle])


