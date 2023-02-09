/**
* \file hint_collecter.cpp
* \brief This files contains code for the 'hint_collector' node.
* \author Shozab Abidi
* \version 1.0
* \date 13/04/2022
*
* \details
* 
* Services : <BR>
* /request_hint_collector
* /hint_loader_service
* 
* Publishers / Subscribers : <BR>
* /oracle_hint
*  
* Description :
*	This node provides the '/request_hint_collector' service which collects and store the hints from the '/oracle_hint' topic. 
* Once three hints are collected, it checks their consistency and load them in the 'ARMOR' ontology knowlegde base. It start the
* ontology reasoner, and check if the deduced hypothesis is complete and correct. If the hypothesis is inconsistent, incomplete or
* incorrect then it return 'false' otherwise it return 'true'.
*  
*/

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <std_msgs/Float64.h>
#include <sstream>
#include <iostream>
#include <string>
#include "erl2/ErlOracle.h"
#include "erl2/HintLoader.h"
#include <std_srvs/Empty.h>
#include <erl2/HintCollector.h>
#include <cstdlib>
#include <unistd.h>


///< Global variable used for collecting hint1 value.
erl2::ErlOracle hint1;
///< Global variable used for collecting hint2 value.
erl2::ErlOracle hint2;
///< Global variable used for collecting hint2 value.
erl2::ErlOracle hint3;

///< Initializing ROS Service 'hint_loader_service' client variable. 
ros::ServiceClient hint_loader_client;

///< Global variable used to keep the count of total hints collected.
std::int64_t count = 0;

/**
* \brief A callback function for the '/oracle_hint' topic subscriber.
* \param msg function arguement of type 'erl2::ErlOracle'.
* \return none (function type is void)
*
* This function store the hints in the global variables that being publish in the '/oracle_hint' topic .
* 
*/
void oracle_hint_Callback(const erl2::ErlOracle::ConstPtr& msg)
{
	//ros::AsyncSpinner spinner(1);
  //spinner.start();
	
	if(count == 0)
	{	
		count = 1;
	}
	
	if(count == 1)
	{
		hint1.ID = msg->ID;
		hint1.key = msg->key;
		hint1.value = msg->value;
		//ROS_INFO("Assigning values to Hint1: ID: %s, key: %s, value: %s", hint1[0].c_str(), 
																									//hint1[1].c_str(), hint1[2].c_str());
	}
	
	else if(count == 2)
	{
		hint2.ID = msg->ID;
		hint2.key = msg->key;
		hint2.value = msg->value;
		//ROS_INFO("Assigning values to Hint2: ID: %s, key: %s, value: %s", hint2[0].c_str(), 
																									//hint2[1].c_str(), hint2[2].c_str());		
	}
	
	else if(count == 3)
	{
		hint3.ID = msg->ID;
		hint3.key = msg->key;
		hint3.value = msg->value;
		//ROS_INFO("Assigning values to Hint3: ID: %s, key: %s, value: %s", hint3[0].c_str(), 
																									//hint3[1].c_str(), hint3[2].c_str());		
	}
	
	ROS_INFO("count :%ld\n",count);
}

/**
* \brief A function used for checking the consistency between all collected hints.
* \param none
* \return returns a bool value based on the outcome of the consistency test.
*
* This function checks if all three hints have same IDs and have properly filled 'key' and 'value' fields. If these conditions are not met then
* then the hints are consider inconsistent.
* 
*/
bool check_consistency()
{
	if(hint1.ID == hint2.ID && hint1.ID == hint3.ID)
	{
		if(hint1.key != "" && hint2.key != "" && hint3.key != "")
		{
			if(hint1.key != "-1" && hint2.key != "-1" && hint3.key != "-1")
			{
				if(hint1.value != "" && hint2.value != "" && hint3.value != "")
				{
					if(hint1.value != "-1" && hint2.value != "-1" && hint3.value != "-1")
					{ return true; }
					else
					{ return false; }			
				}
				else
				{ return false; }
			}
			else
			{ return false; }
		}
		else
		{ return false; }
	}
	else
	{ return false; }
}

/**
* \brief A function used for loading the hints in ontology knowlegde base. 
* \param none
* \return returns a bool variable; 'True' if the hints are successfully loaded in the knowlegde base. 
*
* This function load hints in the ontology knowlegde base using the 'hint_loader_service' service.
* 
*/
bool load_hint()
{
	
	erl2::HintLoader hint_loader_srv;
	hint_loader_srv.request.req.ID = hint1.ID;
	hint_loader_srv.request.req.key = hint1.key;
	hint_loader_srv.request.req.value = hint1.value;
	ROS_INFO("loading hint1.");
	if (hint_loader_client.call(hint_loader_srv))
	{
		if(hint_loader_srv.response.res)
		{
			ROS_INFO("Successfully loaded hint1.");
		}
	}
	else
	{
	  ROS_ERROR("Failed to call service request_hint_collector for hint1");
	  return false;
	}

	if(hint_loader_srv.response.res)
	{
		hint_loader_srv.request.req.ID = hint2.ID;
		hint_loader_srv.request.req.key = hint2.key;
		hint_loader_srv.request.req.value = hint2.value;
		ROS_INFO("loading hint2.");
		if (hint_loader_client.call(hint_loader_srv))
		{
			if(hint_loader_srv.response.res)
			{
				ROS_INFO("Successfully loaded hint2.");
			}
		}
		else
		{
		  ROS_ERROR("Failed to call service request_hint_collector for hint2");
		  return false;
		}	
	}

	
	if(hint_loader_srv.response.res)
	{
		hint_loader_srv.request.req.ID = hint3.ID;
		hint_loader_srv.request.req.key = hint3.key;
		hint_loader_srv.request.req.value = hint3.value;
		ROS_INFO("loading hint3.");
		if (hint_loader_client.call(hint_loader_srv))
		{
			if(hint_loader_srv.response.res)
			{
				ROS_INFO("Successfully loaded hint3.");
				return true;
			}
		}
		else
		{
		  ROS_ERROR("Failed to call service request_hint_collector for hint3");
		  return false;
		}	
	}
}

/**
* \brief A function used for starting the ontology reasoner. 
* \param none
* \return returns a bool variable; 'True' if the hints are successfully loaded in the knowlegde base. 
*
* This function start the ARMOR reasoner using the 'hint_loader_service' service.
* 
*/
bool start_reasoner()
{
	erl2::HintLoader check_correctness_srv;
	check_correctness_srv.request.req.ID = -2;
	check_correctness_srv.request.req.key = "REASON";
	check_correctness_srv.request.req.value = "";
	if (hint_loader_client.call(check_correctness_srv))
	{
		if(check_correctness_srv.response.res)
		{
			ROS_INFO("Successfully start the reasoner.");
			return true;
		}
	}
	else
	{
	  ROS_ERROR("Failed to call service request_hint_collector for starting the reasoner");
	  return false;
	}
	
}

/**
* \brief A function used for checking the completeness of the hypothesis.
* \param none
* \return returns a bool variable; 'True' if the deduced hypothesis is complete. 
*
* This function checks if the deduced hypothesis based on the previously load hints is complete or not.
* 
*/
bool check_completeness()
{
	erl2::HintLoader check_completeness_srv;
	check_completeness_srv.request.req.ID = -11;
	check_completeness_srv.request.req.key ="COMPLETED";
	check_completeness_srv.request.req.value = "";
	if (hint_loader_client.call(check_completeness_srv))
	{
		if(check_completeness_srv.response.res)
		{
			//ROS_INFO("Successfully start the reasoner.");
			return true;
		}
	}
	else
	{
	  ROS_ERROR("Failed to call service request_hint_collector for starting the reasoner");
	  return false;
	}
	
}

/**
* \brief A function used for checking the correctness of the hypothesis which is based on the hints collected previously.
* \param none
* \return returns a bool variable.
*
* This function checks the correctness of the deduced hypothesis. A hypothesis is correct if its ID matches with the correct
* hypothesis ID which provided by the 'oracle_solution' service. 
* 
*/
bool check_correctness()
{
	ROS_INFO("checking correctness.");
	erl2::HintLoader check_correctness_srv;
	check_correctness_srv.request.req.ID = -2;
	check_correctness_srv.request.req.key = "CORRECTNESS";
	check_correctness_srv.request.req.value = "";
	if (hint_loader_client.call(check_correctness_srv))
	{
		if(check_correctness_srv.response.res)
		{
			return true;
		}
	}
	else
	{
	  ROS_ERROR("Failed to call service request_hint_collector for starting the reasoner");
	  return false;
	}
}

/**
* \brief This function generates hint statement. 
* \param none
* \return	returns a string 
*
* This function generates a hint statement by concatinating the values of previously collected hints. 
* 
*/
std::string getHintStatement()
{
	std::string who_;
	std::string what_;
	std::string where_;

	// hint1
	if(hint1.key == "who")
		who_ = hint1.value;
	else if(hint1.key == "what")
		what_ = hint1.value;
	else if(hint1.key == "where")
		where_ = hint1.value;

	// hint2	
	if(hint2.key == "who")
		who_ = hint2.value;
	else if(hint2.key == "what")
		what_ = hint2.value;
	else if(hint2.key == "where")
		where_ = hint2.value;

	// hint3
	if(hint3.key == "who")
		who_ = hint3.value;
	else if(hint3.key == "what")
		what_ = hint3.value;
	else if(hint3.key == "where")
		where_ = hint3.value;
	
	std::string hintStatement = who_ + std::string(" with the ") + what_ + std::string(" in the ") + where_;
	ROS_INFO("Hint Statement %s",hintStatement.c_str());
	return hintStatement;
	
}

/**
* \brief A callback function for the 'request_hint_collector' service client.
* \param req request arguement of the service 'request_hint_collector' with data type erl2::HintCollector::Request 
* \param res response arguement of the service 'request_hint_collector' with data type erl2::HintCollector::Response
* \return returns a bool variable.
*
* This function respond to the client request by storing the hints. If the three hints are successfully collected then it check consistency,
* completeness and correctness between these hints using the apporpiate functions. 
* 
*/
bool collect_hint(erl2::HintCollector::Request  &req,
				  erl2::HintCollector::Response &res)
{
	ROS_INFO("Got the request %s", req.req.c_str());
	
	if(req.req == "collect")
	{	
		if(count == 1)
		{	
			if(hint1.ID >= 0)
			{
				ROS_INFO("Collected the Hint1: ID: %s, key: %s, value: %s",(std::to_string(hint1.ID)).c_str(),
																														(hint1.key).c_str(),(hint1.value).c_str());
				count = count%3;
				count++;
			}
			else
			{ ROS_INFO("Hint1 array is empty."); }
		}	
		else if(count == 2)
		{
			if(hint2.ID >= 0)
			{
				ROS_INFO("Collected the Hint2: ID: %s, key: %s, value: %s",(std::to_string(hint2.ID)).c_str(),
																														(hint2.key).c_str(),(hint2.value).c_str());
				count = count%3;
				count++;
			}
			else
			{ ROS_INFO("Hint2 array is empty."); }
		}	
		else if(count == 3)
		{
			if(hint3.ID >= 0)
			{
				ROS_INFO("Collected the Hint3: ID: %s, key: %s, value: %s",(std::to_string(hint3.ID)).c_str(),
																														(hint3.key).c_str(),(hint3.value).c_str());
				if(check_consistency())
				{
					ROS_INFO("Hint is ready to be loaded.");
					if(load_hint())
					{
							if(start_reasoner())
							{
									if(check_completeness())
									{
											ROS_INFO("Complete hypothesis is found. Now checking correctness."); 
											
											if(check_correctness() == true)
							        {  
												ROS_INFO("Hints are correct.. hurray.."); 	 
												res.hintStatement = getHintStatement();
												res.result = true;
												return true;
											}									
											else
											{ 
												ROS_INFO("Hints are not correct. Lets go again.");
												count = count%3;
												count++;
												res.result = false;
												res.hintStatement = "";
												return false; 
											}
									}
									else 
									{ 
										ROS_INFO("Hints are not complete. Lets go again."); 
										count = count%3;
										count++;
										res.result = false;
										res.hintStatement = "";
										return false; 
									}
							}
							else 
							{ 
								ROS_INFO("There was an error in starting the reasoner");
								return false; 
							}	
					}
					else 
					{ 
						ROS_INFO("There was an error in loading the hints");
						res.result = false;
						res.hintStatement = "";
						return false; 
					}
				}
				else 
				{ 
					ROS_INFO("hints are not consistent. Moving on...");
					count = count%3;
					count++;
					res.result = false;
					res.hintStatement = "";
					return false; 
				}
				count = count%3;
				count++;
			}
			else
			{ 
				ROS_INFO("Hint3 array is empty.");
			}	
		}
		res.hintStatement = "";
		res.result = true;
		return true; 
	}
}

/**
* \brief The main function of the node 'hint_collector'
* \param argc an integer arguement  
* \param an string double pointer arguement.
*
* \return always return 0 as this function cannot fail.
*
* This function initialize the ros node, server for 'request_hint_collector' service and client for 'hint_loader_service' service. 
* 
*/
int main(int argc, char **argv)
{
  ros::init(argc, argv, "hint_collector");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("/oracle_hint", 10, oracle_hint_Callback);
  ros::ServiceServer service = n.advertiseService("request_hint_collector", collect_hint);
  hint_loader_client = n.serviceClient<erl2::HintLoader>("hint_loader_service");
  std::cout << "'request_hint_collector' service is live.." << std::endl;
  ros::spin();
  return 0;
}
