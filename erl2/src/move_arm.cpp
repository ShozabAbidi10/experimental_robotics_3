/**
* \file move_arm.cpp
* \brief This files contains code for the 'move_arm' node.
* \author Shozab Abidi
* \version 1.0
* \date 13/04/2022
*
* \details
* 
* Services : <BR>
* /request_move_arm
*  
* Description :
*
* This node plans and executes robot's arm motion using ROS Moveit library. It initializes a service through which a client can request
* to move robot arm's end-effector to a desired point in Gazebo simulation.
* 
*/

#include <ros/ros.h>
// MoveIt
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>
#include <erl2/MoveArm.h>
#include "std_msgs/String.h"
#include <cstdlib>
#include <unistd.h>

/**
* \brief A callback function for the '/request_move_arm' service client.
* \param req request arguement of the service '/request_move_arm' with data type erl2::MoveArm::Request 
* \param res response arguement of the service '/request_move_arm' with data type erl2::MoveArm::Response
* \return returns a bool variable.
*
* This function respond to the client request by planning and executing robot's arm motion by using ROS Moveit library.
* 
*/
bool move_arm(erl2::MoveArm::Request  &req, erl2::MoveArm::Response &res){

	ros::AsyncSpinner spinner(1);
	spinner.start();
  
	robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
	const moveit::core::RobotModelPtr kinematic_model = robot_model_loader.getModel();
	ROS_INFO("Model frame: %s", kinematic_model->getModelFrame().c_str());
	
	moveit::core::RobotStatePtr kinematic_state(new moveit::core::RobotState(kinematic_model));
	kinematic_state->setToDefaultValues();
	const moveit::core::JointModelGroup* joint_model_group = kinematic_model->getJointModelGroup("arm");
	moveit::planning_interface::MoveGroupInterface group("arm");
	const std::vector<std::string> &joint_names = joint_model_group->getVariableNames();
  
  ROS_INFO("req.wp == %s", req.wp.c_str());
  geometry_msgs::Pose pose1;
  pose1 = req.pose;
  
  ROS_INFO("Setting (x,y,z) == (%f,%f,%f)", pose1.position.x,pose1.position.y,pose1.position.z);
  
  group.setStartStateToCurrentState();
  group.setApproximateJointValueTarget(pose1,"cluedo_link");
  std::vector<double> joint_values;
  double timeout = 0.1;
  
  //ROS_INFO("joint_model_group == %s", joint_model_group->.c_str());
  
  bool found_ik = kinematic_state->setFromIK(joint_model_group, pose1, "cluedo_link", 10, timeout);

  // Now, we can print out the IK solution (if found):
  if (found_ik)
  {
    kinematic_state->copyJointGroupPositions(joint_model_group, joint_values);
    for (std::size_t i = 0; i < joint_names.size(); ++i)
    {
      ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
    }
  }
  else
  {
    ROS_INFO("Did not find IK solution");
  }

  group.setJointValueTarget(joint_values);
  group.setStartStateToCurrentState();
  group.setGoalOrientationTolerance(0.01);
  group.setGoalPositionTolerance(0.01);

  // Plan and execute
  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  group.plan(my_plan);
  group.execute(my_plan);
    
  std::cout << "Position 1 -> IK + setJointValue" << std::endl;
  sleep(10.0);

// standing straight

  pose1.position.y = 0.0;
  pose1.position.z = 1.6;
  
  ROS_INFO("Setting (x,y,z) == (%f,%f,%f)", pose1.position.x,pose1.position.y,pose1.position.z);
  
  group.setStartStateToCurrentState();
  group.setApproximateJointValueTarget(pose1,"cluedo_link");
  
  //ROS_INFO("joint_model_group == %s", joint_model_group->.c_str());
  
  found_ik = kinematic_state->setFromIK(joint_model_group, pose1, "cluedo_link", 10, timeout);

  // Now, we can print out the IK solution (if found):
  if (found_ik)
  {
    kinematic_state->copyJointGroupPositions(joint_model_group, joint_values);
    for (std::size_t i = 0; i < joint_names.size(); ++i)
    {
      ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
    }
  }
  else
  {
    ROS_INFO("Did not find IK solution");
  }

  group.setJointValueTarget(joint_values);
  group.setStartStateToCurrentState();
  group.setGoalOrientationTolerance(0.01);
  group.setGoalPositionTolerance(0.01);

  // Plan and execute
  //moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  group.plan(my_plan);
  group.execute(my_plan);
    
  std::cout << "Position 1 -> IK + setJointValue" << std::endl;
  sleep(10.0);  
  	
	res.res = true;
	return true;    
}

/**
* \brief The main function of the node 'replan_sub'
* \param argc an integer arguement  
* \param an string double pointer arguement.
*
* \return always return 0 as this function cannot fail.
*
* This function initialize the ros nodehandle and 'request_move_arm' service. 
* 
*/
int main(int argc, char** argv)
{
  ros::init(argc, argv, "robot_model_and_robot_state_tutorial");
  ros::NodeHandle nh;
  ros::ServiceServer service = nh.advertiseService("request_move_arm", move_arm);
  std::cout << "'request_move_arm' service is live.." << std::endl;
  ros::AsyncSpinner spinner(1);
  spinner.start();
  ros::waitForShutdown();
  return(0);
}
  

