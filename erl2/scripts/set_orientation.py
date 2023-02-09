#! /usr/bin/env python

## @package erl2
# \file set_orientation.py
# \brief This file contains code for set_orientation node.
# \author Shozab Abidi
# \version 1.0
# \date 13/04/2022
#
# \details
#
# Service : <BR>
# ° /armor_interface_srv
# ° /oracle_service
#  
# This node recieves the desired orientation goal coordinate as a '/request_set_orientation' service request from the 'my_action' node.
# Based on the recieved goal coordinates it computes the required angular velocity of the robot to reach the desired orientation and then
# publishes it in 'cmd_vel' topic.
#

import rospy
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist, Point, Pose
from erl2.srv import SetOrien
from nav_msgs.msg import Odometry
from tf import transformations
import math
import actionlib
import actionlib.msg
import motion_plan.msg

##  A global variable with datatype 'int' used for storing robot's yaw value.
yaw_ = 0

##  A global variable with datatype float, used as a parameter for robot's yaw precision.
yaw_precision_ = math.pi / 9  # +/- 20 degree allowed
##  A global variable with datatype float, also used as a parameter for robot's yaw precision.
yaw_precision_2_ = math.pi / 90  # +/- 2 degree allowed

##  A global variable used for initializing the publisher for topic '/cmd_vel'.
pub = None

##  A global variable with datatype float used as a parameter for generating robot's new velocities.
kp_a = -3.0  
##  A global variable with datatype float used as a parameter for generating robot's new velocities.
kp_d = 0.2
##  A global variable with datatype float used as a parameter for generating robot's new velocities.
ub_a = 0.6
##  A global variable with datatype float used as a parameter for generating robot's new velocities.
lb_a = -0.5
##  A global variable with datatype float used as a parameter for generating robot's new velocities.
ub_d = 0.6

##
# \brief This is a callback function of the subscriber 'sub_odom' for the ROS topic '/odom'
# \param msg arguement with data structure type Odometry  
# \return [none]
#
# This function takes the robot's odometry data coming from the '/odom' topic, 
# extract the position and yaw and then store it in the global variables 'position_' 
# and 'yaw_'. 
#
def clbk_odom(msg):
    global position_
    global pose_
    global yaw_

    # position
    position_ = msg.pose.pose.position
    pose_ = msg.pose.pose

    # yaw
    quaternion = (
        msg.pose.pose.orientation.x,
        msg.pose.pose.orientation.y,
        msg.pose.pose.orientation.z,
        msg.pose.pose.orientation.w)
    euler = transformations.euler_from_quaternion(quaternion)
    yaw_ = euler[2]
    #print(yaw_)
    
##
# \brief This is a function used for normalizing the orientation angle of robot from goal orientation. 
# \param angle arguement with data type 'float'  
# \return Normalized angle value.
#
# This is a function used for normalizing the orientation angle of robot from goal orientation. 
#
def normalize_angle(angle):
    if(math.fabs(angle) > math.pi):
        angle = angle - (2 * math.pi * angle) / (math.fabs(angle))
    return angle

##
# \brief This function adjust the yaw of the robot. 
# \param des_pos arguement with datatype 'float'.  
# \return [none].
#
# This function takes the desired position data as argument. Based on the 'yaw error' calculates the
# required angular velocity and then publishes it in the topic 'cmd_vel'. 
#
def fix_yaw(goal):
		
		global yaw_, pub, yaw_precision_2_
		desired_yaw = goal
		err_yaw = normalize_angle(desired_yaw - yaw_)
		
		while not (math.fabs(err_yaw) <= yaw_precision_2_):
				err_yaw = normalize_angle(desired_yaw - yaw_)
				twist_msg = Twist()

				print("yaw_ :" + str(yaw_))
				print("desired_yaw_ :" + str(desired_yaw))
				print("err_yaw_ :" + str(err_yaw))
				print("twist_msg.angular.z_ :" + str(twist_msg.angular.z))	

				if math.fabs(err_yaw) > yaw_precision_2_:
						twist_msg.angular.z = kp_a*err_yaw
						if twist_msg.angular.z > ub_a:
								twist_msg.angular.z = ub_a
						elif twist_msg.angular.z < lb_a:
								twist_msg.angular.z = lb_a

				pub.publish(twist_msg)

			# state change conditions
			#if math.fabs(err_yaw) <= yaw_precision_2_:
		#print ('Yaw error: [%s]' % err_yaw)
		return done()
		
##
# \brief This is function makes the angular velocities of robot to zero. 
# \param [none]   
# \return [none].
#
# This is function makes the angular velocities to zero. Generally it use when robot
# has successfully reach the desired orientation.  
#
def done():
    twist_msg = Twist()
    twist_msg.angular.z = 0
    pub.publish(twist_msg)
    return True

##
# \brief This is a callback function of 'request_set_orientation' service. 
# 
# \return Bool.
#
# This function respond to the client request by calling the 'set_orientation' function to set the robot's orientation.
#
def set_orien_clbk(msg):
		success = fix_yaw(msg.goal)
		return success

##
# \brief The main function of the node 'hint_collector'
# 
# \return always return 0 as this function cannot fail.
#
# This function initialize the ros node, server for '/request_set_orien' service, client for '/odom' topic and publisher for '/cmd_vel' topic. 
#
def main():
	global pub
	rospy.init_node('set_orientation_server')
	pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
	sub_odom = rospy.Subscriber('/odom', Odometry, clbk_odom)
	set_orien_server = rospy.Service('/request_set_orientation',SetOrien, set_orien_clbk)
	print ('set orientation service is live...')
	rate = rospy.Rate(20)
	while not rospy.is_shutdown():
		rate.sleep()

if __name__ == '__main__':
    main()
