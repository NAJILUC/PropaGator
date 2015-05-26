#! /usr/bin/env python

import rospy
from uf_common.msg import PoseTwistStamped
from uf_common.orientation_helpers import xyz_array
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Vector3Stamped
from std_msgs.msg import Header
import tools
import numpy as np
import math
import tf


class wrench_generator:
	def __init__(self, name):
		# Current pose
		self.carrot_position = self.current_position = np.zeros(3)
		self.carrot_position = self.current_orientation = np.zeros(3)
		self.current_velocity = np.zeros(3)

		# Tf stuff
		self.tf_listener = tf.TransformListener()
		#self.tf_listener.waitForTransform('/enu', '/base_link', rospy.Time(0), rospy.Time(10000))

		# Wait for current position and set as desired position
		rospy.loginfo('Waiting for /odom')
		self.odom_sub = rospy.Subscriber('/odom', Odometry, self.odom_cb, queue_size = 10)
		while not self.current_position.any():	# Will be 0 until an odom publishes (if its still 0 it will drift very very soon)
			pass

		# Start traj subscriber
		self.traj_sub = rospy.Subscriber('/trajectory', PoseTwistStamped, self.traj_cb, queue_size = 10)

	# Calculate error
	def traj_cb(self, traj):
		# Get vectors related to the orientation of the trajectory
		o = tools.normal_vector_from_posetwist(traj.posetwist)
		o_hat = o / np.linalg.norm(o)
		o_norm = np.cross([0, 0, 1], o)

		# Velocity error = velocity perpendicular to trajectory orientation
		velocity_error = self.current_velocity - (np.dot(self.current_velocity, o_hat) * o_hat)
		v_dir = np.dot(velocity_error, o_norm)
		velocity_error = math.copysign(np.linalg.norm(velocity_error), v_dir)
		#print 'V = ' + str(self.current_velocity)
		#print 'V_perp = ' + str(velocity_error)

		# Acceleration error
		#self.acceleration_error = self.velocity_error - self.velocity_error


	# Update pose and twist
	def odom_cb(self, msg):
		# Pose is relative to /enu!!!!!!!!
		# Twist is relative to /base_link!!!!!!!!!
		print msg
		self.current_position = tools.position_from_pose(msg.pose.pose)
		self.current_orientation = tools.orientation_from_pose(msg.pose.pose)

		# Transform velocity from boat frame of reference to enu
		vec = Vector3Stamped(
				header = Header(
					frame_id = '/base_link',
					stamp = msg.header.stamp),
				vector = msg.twist.twist.linear)
		try:
			#										from 			to 			at time
			#(trans,rot) = tf_listener.lookupTransform('/base_link', '/enu', rospy.Time(0))
			#print rot
			rospy.loginfo('Vec before transform: ' + str(vec))
			vec = self.tf_listener.transformVector3('/enu', vec)
			rospy.loginfo('Vec after transform: ' + str(vec))
		except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException) as err:
			rospy.logwarn('Tf error: ' + str(err))
			return

		self.current_velocity = xyz_array(vec.vector)

if __name__ == '__main__':
	rospy.init_node('wrench_generator')
	node = wrench_generator(rospy.get_name())
	rospy.spin()