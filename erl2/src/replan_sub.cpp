/**
* \file replan_sub.cpp
* \brief This files contains code for the 'replan_sub' node.
* \author Shozab Abidi
* \version 1.0
* \date 13/04/2022
*
* \details
* 
* Publishers / Subscribers : <BR>
* /replan
*  
* Description :
*
* This node initializes a subscriber for '/replan' topic. The callback funtion of this subscriber execute the 'rosplan_start.sh' bash file and
* starts the replanning.
*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdio.h>
#include <stdlib.h>


/**
* \brief A callback function for the 'replan' topic subscriber.
* \param req takes std_msgs::String as arguement which is the type of the "replan" topic.
* \return none (function type is void)
*
* This function starts the replanning process by executing 'rosplan_start.sh' bash file.
* 
* 
*/
void replan_callback(const std_msgs::String::ConstPtr& msg)
{
	if(msg->data == "replan")
	{
		ROS_INFO("Starting replaning");
		system("/root/ros_ws/src/erl2/scripts/rosplan_start.sh"); 
	}
}

/**
* \brief The main function of the node 'replan_sub'
* \param argc an integer arguement  
* \param an string double pointer arguement.
*
* \return always return 0 as this function cannot fail.
*
* This function initialize the ros node and subscriber for 'replan' topic. 
* 
*/
int main(int argc, char **argv)
{
  ros::init(argc, argv, "replan_sub");
  ros::NodeHandle n;
  ROS_INFO("'replan_sub' node is live...");
  ros::Subscriber sub = n.subscribe("replan", 100, replan_callback);
  ros::spin();
  return 0;
}
