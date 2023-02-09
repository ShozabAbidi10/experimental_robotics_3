/**
* \file replan.cpp
* \brief This files contains code for the 'replan' node.
* \author Shozab Abidi
* \version 1.0
* \date 13/04/2022
*
* \details
* 
* Services : <BR>
* /request_replan
* 
* Publishers / Subscribers : <BR>
* /replan
*  
* Description :
* 
*	This node provides the '/request_replan' service which signal the 'replan_sub' node to start the replanning process by publishing
* the string "replan" in the topic 'replan'. 
*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <sstream>
#include <iostream>
#include <string>
#include <std_srvs/Empty.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

///< Initializing ROS Publisher 'replan_pub_' variable for '\replan' topic.
ros::Publisher replan_pub_;


/**
* \brief A callback function for the '/request_replan' service client.
* \param req request arguement of the service '/request_replan' with data type std_srvs::Empty::Request 
* \param res response arguement of the service '/request_replan' with data type std_srvs::Empty::Response
* \return returns a bool variable.
*
* This function respond to the client request by publishing the string "replan" in the topic. This will signal the subscriber of the same topic to
* start replanning by executing 'rosplan_start.sh' bash file.
* 
*/
bool replan_srv_callback(std_srvs::Empty::Request &req, std_srvs::Empty::Response &res)
{
	std_msgs::String msg;
	msg.data = "replan";
	replan_pub_.publish(msg);
	return true;
}

/**
* \brief The main function of the node 'replan'
* \param argc an integer arguement  
* \param an string double pointer arguement.
*
* \return always return 0 as this function cannot fail.
*
* This function initialize the ros node, server for '/request_replan' service and publisher for 'replan' topic. 
* 
*/
int main(int argc, char **argv)
{
  ros::init(argc, argv, "replan");
  ros::NodeHandle n;
  ROS_INFO("'request_replan' service is live...");
  ros::ServiceServer service = n.advertiseService("/request_replan", replan_srv_callback);
	replan_pub_ = n.advertise<std_msgs::String>("replan", 100);
  ros::spin();
  return 0;
}
