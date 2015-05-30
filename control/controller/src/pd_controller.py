#!/usr/bin/env python

#reference paper = Nonlinear Control of an Autonomous Underwater Vehicle: A RISE-Based Approach

import roslib
roslib.load_manifest('controller')
import rospy
from geometry_msgs.msg import WrenchStamped, Vector3, Vector3Stamped, Point, Wrench, PoseStamped
from std_msgs.msg import Header,Bool, Float64
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
import numpy,math,tf,threading
from tf import transformations
from uf_common.orientation_helpers import xyz_array, xyzw_array
from uf_common.msg import PoseTwistStamped
from controller.srv import Enable,EnableResponse
from kill_handling.listener import KillListener
from kill_handling.broadcaster import KillBroadcaster

rospy.init_node('controller')
controller_wrench = rospy.Publisher('wrench', WrenchStamped, queue_size = 1)
lock = threading.Lock()

#set controller gains
'''
rospy.set_param('p_gain', {'x':2,'y':2, 'yaw':.15})#5,.8//4.0,.8
rospy.set_param('d_gain', {'x':19,'y':11, 'yaw':20})
#.25,400
#-----------
'''

d_x = 19
d_y = 11
d_z = 20
p_x = 2
p_y = 2
p_z =.15

K = numpy.array([
    [p_x,0,0,0,0,0],
    [0,p_y,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,p_z]])

Ks = numpy.array([
    [d_x,0,0,0,0,0],
    [0,d_y,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,d_z]])

def d_gain_cb(msg):
    global d_z, Ks
    d_z = msg.data
    Ks[5,5] = d_z


def p_gain_cb(msg):
    global p_z, K
    p_z = msg.data
    K[5,5] = p_z

rospy.Subscriber("pd_d_gain", Float64, d_gain_cb)
rospy.Subscriber("pd_p_gain", Float64, p_gain_cb)

'''


def d_gain_cb(msg):
    global d_x, d_y, d_z, Ks
    d_x = msg.x
    d_y = msg.y
    d_z = msg.z

    Ks = numpy.array([
    [d_x,0,0,0,0,0],
    [0,d_y,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,d_z]])

def p_gain_cb(msg):
    global p_x, p_y, p_z, K
    p_x = msg.x
    p_y = msg.y
    p_z = msg.z

    K = numpy.array([
    [p_x,0,0,0,0,0],
    [0,p_y,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,0],
    [0,0,0,0,0,p_z]])

d_gain_sub = rospy.Subscriber("pd_d_gain", Point, d_gain_cb)
p_gain_sub = rospy.Subscriber("pd_p_gain", Point, p_gain_cb)

'''
#----------------------------------------------------------------------------------


# KILL MANAGER SETTINGS -----------------------------------------------------------

killed = False

def set_kill():
    global killed 
    killed = True
    rospy.logwarn('PD_Controller KILLED: %s' % kill_listener.get_kills())

def clear_kill():
    global killed 
    killed = False
    rospy.logwarn('PD_Controller ACTIVE: %s' % kill_listener.get_kills())

kill_listener = KillListener(set_kill, clear_kill)

# Kill publisher on line 179

# basing wrench output on the 'killed' variable

def _jacobian(x):
    # maps body linear+angular velocities -> global linear velocity/euler rates
    sphi, cphi = math.sin(x[3]), math.cos(x[3])
    stheta, ctheta, ttheta = math.sin(x[4]), math.cos(x[4]), math.tan(x[4])
    spsi, cpsi = math.sin(x[5]), math.cos(x[5])
    
    J = numpy.zeros((6, 6))
    J[0:3, 0:3] = [
        [ ctheta * cpsi, -cphi * spsi + sphi * stheta * cpsi,  sphi * spsi + cphi * stheta * cpsi],
        [ ctheta * spsi,  cphi * cpsi + sphi * stheta * spsi, -sphi * cpsi + cphi * stheta * spsi],
        [-stheta       ,                sphi * ctheta       ,                cphi * ctheta       ],]
    
    J[3:6, 3:6] = [
        [1, sphi * ttheta,  cphi * ttheta],
        [0, cphi         , -sphi         ],
        [0, sphi / ctheta,  cphi / ctheta],]
    return J

def _jacobian_inv(x):
    # maps global linear velocity/euler rates -> body linear+angular velocities
    sphi, cphi = math.sin(x[3]), math.cos(x[3])
    stheta, ctheta = math.sin(x[4]), math.cos(x[4])
    spsi, cpsi = math.sin(x[5]), math.cos(x[5])
    
    J_inv = numpy.zeros((6, 6))
    J_inv[0:3, 0:3] = [
        [       ctheta * cpsi              ,        ctheta * spsi              ,        -stheta],
        [sphi * stheta * cpsi - cphi * spsi, sphi * stheta * spsi + cphi * cpsi,  sphi * ctheta],
        [cphi * stheta * cpsi + sphi * spsi, cphi * stheta * spsi - sphi * cpsi,  cphi * ctheta],]
    J_inv[3:6, 3:6] = [
        [1,     0,       -stheta],
        [0,  cphi, sphi * ctheta],
        [0, -sphi, cphi * ctheta],]
    return J_inv

#---------------collect desired state information as soon as it is posted--------------------
global desired_state_set,previous_error,odom_active,enable
enable = True
odom_active = False
desired_state_set = False
desired_state = numpy.zeros(6)
desired_state_dot = numpy.zeros(6)
state = numpy.zeros(6)
previous_error = numpy.zeros(6)
state_dot = numpy.zeros(6)
state_dot_body = numpy.zeros(6)

def desired_state_callback(desired_posetwist):
    global desired_state,desired_state_dot,desired_state_set
    lock.acquire()
    desired_state_set = True
    desired_state = numpy.concatenate([xyz_array(desired_posetwist.posetwist.pose.position), transformations.euler_from_quaternion(xyzw_array(desired_posetwist.posetwist.pose.orientation))])
    desired_state_dot = _jacobian(desired_state).dot(numpy.concatenate([xyz_array(desired_posetwist.posetwist.twist.linear), xyz_array(desired_posetwist.posetwist.twist.angular)]))
    lock.release()

rospy.Subscriber('/trajectory', PoseTwistStamped, desired_state_callback)

#----------------------------------------------------------------------------------





def odom_callback(current_posetwist):
    global desired_state,desired_state_dot,state,stat_dot,state_dot_body,desired_state_set,odom_active
    lock.acquire()
    odom_active = True
    state = numpy.concatenate([xyz_array(current_posetwist.pose.pose.position), transformations.euler_from_quaternion(xyzw_array(current_posetwist.pose.pose.orientation))])
    state_dot = _jacobian(state).dot(numpy.concatenate([xyz_array(current_posetwist.twist.twist.linear), xyz_array(current_posetwist.twist.twist.angular)]))
    state_dot_body = numpy.concatenate([xyz_array(current_posetwist.twist.twist.linear), xyz_array(current_posetwist.twist.twist.angular)])
    if (not desired_state_set):
        desired_state = state
        desired_state_set = True
    lock.release()


def update_callback(event):
    
    global desired_state,desired_state_dot,state,stat_dot,state_dot_body,previous_error,enable
    
    lock.acquire()
    #print 'desired state', desired_state
    #print 'current_state', state
    def smallest_coterminal_angle(x):
        return (x + math.pi) % (2*math.pi) - math.pi

    x_error = abs(abs(desired_state[1]) - abs(state[1]))
    y_error = abs(abs(desired_state[0]) - abs(state[0]))
    z_error = abs((desired_state[5] % 2*math.pi) - ((state[5] + 3.14) % 2*math.pi))

    print "X: ", y_error
    print "Y: ", x_error
    print "Z: ", z_error

    # sub pd-controller sans rise
    e = numpy.concatenate([desired_state[0:3] - state[0:3], map(smallest_coterminal_angle, desired_state[3:6] - state[3:6])]) # e_1 in paper
    vbd = _jacobian_inv(state).dot(K.dot(e) + desired_state_dot)
    e2 = vbd - state_dot_body
    output = Ks.dot(e2)

    
    lock.release()
    if (not(odom_active)):
        output = [0,0,0,0,0,0]
    if (enable & killed==False):
        controller_wrench.publish(WrenchStamped(
            header = Header(
                stamp=rospy.Time.now(),
                frame_id="/base_link",
                ),
            wrench=Wrench(
                force = Vector3(x= output[0],y= output[1],z= 0),
                torque = Vector3(x=0,y= 0,z= output[5]),
                ))
                
                )

        if((x_error < 1) & (y_error < 1) & (z_error < 1)):
                waypoint_progress = rospy.Publisher('waypoint_progress', Bool, queue_size = 1)
                waypoint_progress.publish(True)

    if (killed == True):
        rospy.logwarn('PD_Controller KILLED: %s' % kill_listener.get_kills())
        controller_wrench.publish(WrenchStamped(
                header = Header(
                    stamp=rospy.Time.now(),
                    frame_id="/base_link",
                    ),
                wrench=Wrench(
                    force = Vector3(x= 0,y= 0,z= 0),
                    torque = Vector3(x=0,y= 0,z= 0),
                    ))
                    
                    )

def setStatus(action):
    global enable
    if action.enable:
        enable = True
    else:
        enable = False
    return EnableResponse()
rospy.Service('~enable', Enable, setStatus)

def timeout_callback(event):
    global odom_active
    odom_active = False

rospy.Timer(rospy.Duration(.1),update_callback)
rospy.Timer(rospy.Duration(1),timeout_callback)
rospy.Subscriber('/odom', Odometry, odom_callback)
rospy.spin()
