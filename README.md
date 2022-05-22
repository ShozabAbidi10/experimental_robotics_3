# Project 3 - Experimental Robotics (MSc Robotics Engineering, Unige)
This project is the extention of the work previously done in project 1 and 2 of the experimental robotics course. Details about the previous two projects can be found here: https://github.com/ShozabAbidi10/experimental_robotics_1,  https://github.com/ShozabAbidi10/experimental_robotics_2 . 

## Project Description:

In the previous iteration, a ROS package was developed for a Clauedo game simulation in Gazebo environment in which a robot explore the environment to collect hints and deduced an hypothesis about 'who can be the killer'. Building upon the same idea, in this iteration we have added some more complex features in simulation environment and similarly robot technology stack has been upgraded. The simulation environment in this project contains six rooms each with 5 ArUco markers placed in different locations, in total there are thirty ArUco markers in the entire environment. As each marker corresponds to a hint, robot has to first visit each room, search for markers and scan them. Upon successfully scanning the marker robot will get the marker's id. Using this marker 'id' as a request to service '/oracle_hint' robot can retrive the hint. Once the hint has been received, robot has to load this hint in the Armor reasoner and check for deduced hypothesis that are complete.

Similar to the previous two projects the deduced hypotheses has to be complete and correct. For a hypothesis to be complete it has to be based on three different types of hints (who,where,what). Hypothesis is correct when firstly its complete and secondly its ID needs to match the ID of the correct hypothesis which is decided by the oracle node. The correct ID can be requested from the oracle node using the service '/oracle_solution'. The hints are of following types:

1. who: Robot can find a name of a person as a hint which can be a killer e.g: Prof. Plum.
2. what: Robot can find a name of a weapon as a hint which killer might have used e.g: Dagger.
3. where: Robot can find a name of random place where the crime might have been committed e.g: Hall.

Statement of a consistent hypothesis will be something like this: “Prof. Plum with the Dagger in the Hall”. Incase the deduced hypotheses is wrong then the robot will resume its exploration for new hints until it forms a consistent hypothesis. Similar to the previous project, ARMOR package has been used to deduced the hypothesis which is developed by researchers at University of Genova. ARMOR is versatile management system that can handle single or multiple-ontology architectures under ROS. Please find more details regarding ARMOR from here: https://github.com/EmaroLab/armor

## Project Installation:

This project requires ROS with ARMOR, erl2 (experimental robotics assignment2) and ROS navigation stack move_base packages to be install in the system. Please make sure you have already install it before following the instructions. 

For installing ARMOR package please follow the instructions available in this respository: https://github.com/EmaroLab/armor.

1. Code available in **Main** branch is a ROS-Noetic package which should be place in ROS workspace {ros1_ws}/src after downloading.
2. To successfully deploy and build the package run the following command.
```
catkin_make
source devel/setup.bash
```
3. In order to use the python modules contained in armor_py_api package run the following command to add the path of the armor python modules to your PYTHONPATH environmental variable.
``` 
export PYTHONPATH=$PYTHONPATH:/root/ros_ws/src/armor/armor_py_api/scripts/armor_api/
```
4. Download the 'cluedo_ontology.owl' file provided in this repository and place it in your system '/root/Desktop/' directory. 

## Running the Project Simulation:

1. After successfully installing the python package and other dependecies open a command window and start ROS master by using the following command:
```
roscore&
```
2. After that start the ARMOR service by using the following command:
```
rosrun armor execute it.emarolab.armor.ARMORMainService
```
3. Open the new tab in command terminal and run the ROS launch file to start the simulation by using the following command: 
```
roslaunch exp_assignment3 simulation.launch
```
After running the command wait for the system to load all the files. Once all nodes are loaded open another terminal and execute 'assignment_services' launch file by running the following command:
```
roslaunch erl2 assignment_services.launch
```

## Software Architecture of the Project:

The project architecture is based on the following main nodes. 

![rqt_graph_assignment3](https://user-images.githubusercontent.com/61094879/169672250-8b9784d6-d6af-4899-b74e-806bf6a3fae9.PNG)

1. final_oracle | simulation.cpp

The assignment3 node implement the two 'oracle' services '/oracle_hint' and '/oracle_solution'. The oracle is implemented in a way that it randomly generate hints of IDs 0 to 5 which may or may not generate inconsistent hypothesis. It also randomly generates a trustable ID; only the hypothesis deduced from hints with trustable ID are considered correct hypothesis. This trustable ID can be requested from the 'oracle_solution' service. The generated hints by this node are publish in '/oracle_hint' topic.

2. simple_action | simple_action.cpp

This node implements a state machine which curates the robot behavior. The node recieves the odometry data of the robot from the topic '/odom' and keeps the track of robot's position in the simulation environment. Based on robot's position the state machine updates in the following states, 1) Reach center of the room[x]. 2) start exploration in room[x] and collect hint. 3) If hint is detected then request '/hint_collector' service to collect and load the hint in armor knowlegde base (KB). For allowing the robot to move from one room to another while avoiding the obstacles move_base action client is implemented.  

3. hint_collector | hint_collector.cpp 

This node provides the '/request_hint_collector' service which collects and store the hints from the '/oracle_hint' service. Once the hint is collected, the node checks its consistency and load them in the 'ARMOR' ontology knowlegde base (KB). It start the ontology reasoner, and check if the deduced hypothesis based on the previously loaded hints is complete and correct. If the hypothesis is inconsistent, incomplete or incorrect then it return 'false' otherwise it return 'true'.

4. hint_loader | hint_loader.py

This node waits for 'hint_loader_service' service's request from the 'hint_collector' node. Based on the request recieved, it loads the hint in the ARMOR knowlegde base, start the reasoner to deduced a hypotheses based on the previously loaded hints and request ARMOR reasoner for the list of 'COMPLETE' hypotheses. If the recently deduced hypothesis is 'COMPLETE' then it checks its 'CORRECTNESS' and return the appropiate response to the 'hint_collector' node. 

5. follow_marker_service | follow_marker_service.py

This node waits for 'hint_loader_service' service's request from the 'simple_action' node to start the exploration behavior; in which the robot starts rotating on its own axis until it sees aruco marker in its field of vision. Once a marker is detected, the robot starts moving closer to the marker until it is able to scan the marker. Once the marker is scaned and its 'id' has been retrive the exploration behavior stops and the node returns the marker's id as response to the service request.

6. marker_publisher_cam1 | marker_publish_cam1.cpp

This node uses raw images of the environment captures from camera1 of the robot and ArUco libraries to scan ArUco marker in the Gazebo simulation. Once the marker is scaned it publishes the marker's id in the topic '/aruco_marker_id'. 

7. marker_publisher_cam2 | marker_publish_cam2.cpp

Similar to previous node, this node uses raw images of the environment captures from camera2 of the robot and ArUco libraries to scan ArUco marker in the Gazebo simulation. Once the marker is scaned it publishes the marker's id in the topic '/aruco_marker_id'. 

## Project Simulation Demo:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/4idP0vozB_s/0.jpg)](https://www.youtube.com/watch?v=4idP0vozB_s)

## Code Documentation:

The code documentation is done using Doxygen tool. Please find the doxygen documentation in the **main** branch .

## Contant Info: 
1. Author: Shozab Abidi
2. Email: hasanshozab10@gmail.com
