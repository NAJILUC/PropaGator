#!/usr/bin/env python
'''

This source is written for use in the Machine Intelligence Lab in the MAE
Department at the University of Florida. 
It is writen for use on the UF PropaGator robot
It is released under the BSD license and is intended for university use
This code is provided "as is" and relies on specific hardware, use at your own risk

Title: PID position controller
Start Date: 08-22-2015

Author: Zach Goins
Author email: zach.a.goins@gmail.com

Co-author:
Co-author email:

CODE DETAILS --------------------------------------------------------------------

Please include inputs, outputs, and fill with a pseudo-code or description of the source to follow

inputs: /trajectory, /odom, /pid_d_gain, /pid_p_gain, /pid_i_gain
output: /wrench, 

This file is used to take the trajectory given by the path planner
and command the wrenches to be given the the thruster mapper

1. Get desired position -> ROS message callback
2. Get current position -> ROS message callback
3. Computer linear and angular error in /enu
4. Convert the errors to /baselink using defined transformation matrix
5. Use PID controller to computer desired wrench and send 

'''

import rospy
import roslib
roslib
roslib.load_manifest('controller')
import rospy
from geometry_msgs.msg import WrenchStamped, Vector3, Vector3Stamped, Point, Wrench, PoseStamped
from std_msgs.msg import Header,Bool, Float64
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
import numpy,math,tf,threading
from tf import transformations
from uf_common.orientation_helpers import xyz_array, xyzw_array, quat_to_rotvec
from uf_common.msg import PoseTwistStamped
from controller.srv import Enable, EnableResponse
from kill_handling.listener import KillListener
from kill_handling.broadcaster import KillBroadcaster
from collections import deque

rospy.init_node('pd_controller', anonymous=True, log_level=rospy.DEBUG)

p_x = rospy.get_param('~p_x')
p_y = rospy.get_param('~p_y')
p_z = rospy.get_param('~p_z')
i_x = rospy.get_param('~i_x')
i_y = rospy.get_param('~i_y')
i_z = rospy.get_param('~i_z')
d_x = rospy.get_param('~d_x')
d_y = rospy.get_param('~d_y')
d_z = rospy.get_param('~d_z')

class PID_controller:

    def __init__(self):

        '''

        Structure gain array for flexible use
        This allows the gain function to access all gains by simple indexing scheme
        Place x and y gains in the first row of the 6x3
        Place x gains in the bottom row
        the i_history array uses this scheme as well

        Gain array layout:

            [  p_x.   i_x.   d_x.]
            [  p_y.   i_y.   d_y.]
            [  0.      0.      0.]
            [  0.      0.      0.]
            [  0.      0.      0.]
            [  p_z.   i_z.   d_z.]

        '''

        self.K = numpy.zeros((6,3))
        self.K[0:6, 0:6] = [
            [p_x,  i_x, d_x], # pid_x
            [p_y, i_y, d_y], # pid_y
            [0,   0,  0],
            [0,   0,  0],
            [0,   0,  0],
            [p_z, i_z, d_z], # pid_z
        ]

        # Kill functions
        self.odom_active = False
        self.killed = False

        # Create arrays to be used
        self.desired_state = numpy.ones(6)
        self.desired_velocity = numpy.ones(6)
        self.current_state = numpy.zeros(6)
        self.current_velocity = numpy.zeros(6)
        self.current_error = numpy.ones(6)

        self.i_history = [[0 for x in range(1)] for x in range(6)] 
        self.i_history[0] = deque()
        self.i_history[1] = deque()
        self.i_history[5] = deque()
        self.integrator = numpy.zeros(6)
        self.Derivator= 0

        self.lock = threading.Lock()

        # Used to reset the desired state on startup
        self.desired_state_set = False

        # ROS components
        self.controller_wrench = rospy.Publisher('wrench', WrenchStamped, queue_size = 1)
        self.kill_listener = KillListener(self.set_kill, self.clear_kill)
        rospy.Subscriber('/trajectory', PoseTwistStamped, self.trajectory_callback)
        rospy.Subscriber('/odom', Odometry, self.odom_callback)
        rospy.Subscriber("pid_d_gain", Point, self.d_gain_callback)
        rospy.Subscriber("pid_p_gain", Point, self.p_gain_callback)
        rospy.Subscriber("pid_i_gain", Point, self.i_gain_callback)

    def p_gain_callback(self, msg):
        x = msg.x
        y = msg.y
        z = msg.z

        self.K[0:2, 0] = [x,y]
        self.K[5, 0] = z

    def i_gain_callback(self, msg):
        x = msg.x
        y = msg.y
        z = msg.z

        self.K[0:2, 1] = [x,y]
        self.K[5, 1] = z

    def d_gain_callback(self, msg):
        x = msg.x
        y = msg.y
        z = msg.z

        self.K[0:2, 2] = [x,y]
        self.K[5, 2] = z

    def set_kill(self):
        self.killed = True
        rospy.logdebug('PD_Controller KILLED: %s' % self.kill_listener.get_kills())

    def clear_kill(self):
        self.killed = False
        rospy.logdebug('PD_Controller ACTIVE: %s' % self.kill_listener.get_kills())

    def trajectory_callback(self, desired_trajectory):
        self.lock.acquire()
        self.desired_state_set = True
        # Get desired pose and orientation 
        desired_pose = xyz_array(desired_trajectory.posetwist.pose.position)
        desired_orientation = transformations.euler_from_quaternion(xyzw_array(desired_trajectory.posetwist.pose.orientation))
        # Get desired linear and angular velocities
        desired_lin_vel = xyz_array(desired_trajectory.posetwist.twist.linear)
        desired_ang_vel = xyz_array(desired_trajectory.posetwist.twist.angular)
        # Add desired position to desired state i_historys
        self.desired_state = numpy.concatenate([desired_pose, desired_orientation])
        # Add desired velocities to velocity i_history
        self.desired_velocity = numpy.concatenate([desired_lin_vel, desired_ang_vel])
        self.lock.release()

    def odom_callback(self, current_pos):
        self.lock.acquire()
        self.odom_active = True
        # Grab current position and orientation and 0 linear Z and angluar X and Y
        # Get current position 
        current_position = xyz_array(current_pos.pose.pose.position)
        current_orientation = numpy.array(transformations.euler_from_quaternion(xyzw_array(current_pos.pose.pose.orientation)))
        # Zero unneccesary elements
        current_position[2] = 0
        current_orientation[0:2] = 0
        # Add current position to state i_history
        self.current_state = numpy.concatenate([current_position, current_orientation])
        # Get current velocities
        current_lin_vel = xyz_array(current_pos.twist.twist.linear)
        current_ang_vel = xyz_array(current_pos.twist.twist.angular)
        # Add current velocities to velocity i_history
        self.current_velocity = numpy.concatenate([current_lin_vel, current_ang_vel])
        # If the desired state has not yet been set, set desired and current as the same
        # Resets the controller to current position on bootup
        if (not self.desired_state_set):
            self.desired_state = self.current_state
            self.desired_state_set = True

        self.lock.release()

    def PID(self, variable):

        # Index in state number we want to access
        state_number = 0
        if variable == 'x': state_number = 0
        if variable == 'y': state_number = 1
        if variable == 'z': state_number = 5

        #self.current_error = self.desired_state[state_number] - self.current_state[state_number]
        #rospy.logdebug(variable + ": " + str(self.current_error[state_number]))
        p = self.K[state_number, 0] * self.current_error[state_number]
        i = (self.integrator[state_number] + self.current_error[state_number]) * self.K[state_number, 1]
        d = self.K[state_number, 2] * (self.current_error[state_number] - self.Derivator)

        # This section will be the FOPID implimentation, but I am still working on it
        if abs(self.current_error[state_number]) > 0: pass
            #i = i * (1 + abs(d))

        rospy.logdebug(self.current_error[state_number])
        rospy.logwarn('P' + variable + ": " + str(p))
        rospy.logwarn('I' + variable + ": " + str(i))
        rospy.logwarn('D' + variable + ": " + str(d))

        # Set temporary variable for use in integrator sliding window
        sliding_window = self.i_history[state_number]

        # append to integrator array
        sliding_window.append(i)

        # If array is larger than 5 items, remove item
        if len(sliding_window) > 5:
            sliding_window.pop()

        # Set up variables for next iteration
        # Sum only last 5 numbers of intergration
        self.Derivator = self.current_error[state_number]
        self.integrator[state_number] = sum(sliding_window)

        PID = p + i + d
        return PID

    def timeout_callback(self, event):
        self.odom_active = False

    def jacobian(self, x):
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

    def main_loop(self, event):

        def smallest_coterminal_angle(x):
            return (x + math.pi) % (2*math.pi) - math.pi

        self.lock.acquire()

        # Get linear and angular error between desired and current pose - /enu
        linear_error = self.desired_state[0:3] - self.current_state[0:3]
        angular_error = map(smallest_coterminal_angle, self.desired_state[3:6] - self.current_state[3:6])

        # Combine errors into one array
        error_enu = numpy.concatenate([linear_error, angular_error])
        #rospy.logdebug(error_enu)
        # Translate /enu errors into /base_link errors
        error_base = self.jacobian(self.current_state).dot(error_enu)
        # Take away velocity from error to avoid overshoot
        final_error = error_base - self.current_velocity
        # Place errors to be sent into main error array
        self.current_error = [final_error[0], final_error[1], 0, 0, 0, final_error[5]]

        self.lock.release()

        # Send error values through PID controller
        x = self.PID('x')
        y = self.PID('y')
        z = self.PID('z')

        # Combine into final wrenches
        wrench = [x,y,0,0,0,z]

        # If odometry has not been aquired, set wrench to 0
        if (not(self.odom_active)):
            wrench = [0,0,0,0,0,0]

        # If ready to go...
        if (self.killed == False):
            self.controller_wrench.publish(WrenchStamped(
                header = Header(
                    stamp=rospy.Time.now(),
                    frame_id="/base_link",
                    ),
                wrench=Wrench(
                    force = Vector3(x= wrench[0],y= wrench[1],z= 0),
                    torque = Vector3(x=0,y= 0,z= wrench[5]),
                    ))
                    
                    )

        # If not ready to go...
        if (self.killed == True):
            rospy.logdebug('PD_Controller KILLED: %s' % self.kill_listener.get_kills())
            self.controller_wrench.publish(WrenchStamped(
                    header = Header(
                        stamp=rospy.Time.now(),
                        frame_id="/base_link",
                        ),
                    wrench=Wrench(
                        force = Vector3(x= 0,y= 0,z= 0),
                        torque = Vector3(x=0,y= 0,z= 0),
                        ))
                        
                        )

if __name__ == "__main__":

    controller = PID_controller()
    #rospy.on_shutdown(controller.shutdown)
    rospy.Timer(rospy.Duration(1.0/50.0), controller.main_loop)
    rospy.Timer(rospy.Duration(1), controller.timeout_callback)
    rospy.spin()


