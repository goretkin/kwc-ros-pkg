#pragma once
/*
 *  rosgazebo
 *  Copyright (c) 2008, Willow Garage, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// gazebo
#include <gazebo/gazebo.h>
#include <gazebo/GazeboError.hh>
#include <libpr2API/pr2API.h>
#include <libpr2HW/pr2HW.h>

#include <pr2Controllers/ArmController.h>
#include <pr2Controllers/HeadController.h>
#include <pr2Controllers/SpineController.h>
#include <pr2Controllers/BaseController.h>
#include <pr2Controllers/LaserScannerController.h>
#include <pr2Controllers/GripperController.h>
#include <rosControllers/RosJointController.h>
#include <rosControllers/RosJoint.h>
//When defined, will expose all the joints of the robot through ros
// #define EXPOSE_JOINTS

// for rosController
#include <list>
using std::list;
#include "ringbuffer.h"
#include "mechanism_model/joint.h"
// roscpp
#include <ros/node.h>
// roscpp - used for shutter message right now
#include <std_msgs/Empty.h>
// roscpp - used for broadcasting time over ros
#include <rostools/Time.h>
// roscpp - base
#include <std_msgs/RobotBase2DOdom.h>
#include <std_msgs/BaseVel.h>
// roscpp - arm
#include <std_msgs/PR2Arm.h>

#include <pr2_msgs/EndEffectorState.h>

// for frame transforms
#include <rosTF/rosTF.h>

#include <time.h>

// service
#include <rosgazebo/VoidVoid.h>
#include <rosgazebo/MoveCartesian.h>
#include <rosgazebo/GripperCmd.h>

// xml parser, robot structure
#include <urdf/URDF.h>

// Our node
class RosGazeboNode : public ros::node
{
  private:
    // Messages that we'll send or receive
    std_msgs::BaseVel velMsg;
    std_msgs::RobotBase2DOdom odomMsg;
    rostools::Time timeMsg;
    std_msgs::Empty shutterMsg;  // marks end of a transform

    std_msgs::Point3DFloat32 objectPosMsg;

    // A mutex to lock access to fields that are used in message callbacks
    ros::thread::mutex lock;

    // for frame transforms, publish frame transforms
    rosTFServer tf;
    rosTFClient tfc;

    // time step calculation
    double lastTime, simTime;

    // smooth vx, vw commands
    double vxSmooth, vwSmooth;

    // used to generate Gaussian noise (for PCD)
    double GaussianKernel(double mu,double sigma);

    // used to generate Gaussian noise (for PCD)
    PR2::PR2Robot *PR2Copy;
    controller::ArmController          *armCopy;
    controller::HeadController         *headCopy;
    controller::SpineController        *spineCopy;
    controller::BaseController         *baseCopy;
    controller::LaserScannerController *laserScannerCopy;
    controller::GripperController      *gripperCopy;

    //Copy data from message to Joint Array. Required to be in update for thread locking.
    void UpdateLeftArm();
    void UpdateRightArm();

    robot_desc::URDF pr2Description;

  public:
    void LoadRobotModel();
    // Constructor; stage itself needs argc/argv.  fname is the .world file
    // that stage should load.
    RosGazeboNode(int argc, char** argv, const char* fname,
         PR2::PR2Robot          *myPR2);
    RosGazeboNode(int argc, char** argv, const char* fname,
         PR2::PR2Robot          *myPR2,
         controller::ArmController          *myArm,
         controller::HeadController         *myHead,
         controller::SpineController        *mySpine,
         controller::BaseController         *myBase,
         controller::LaserScannerController *myLaserScanner,
         controller::GripperController      *myGripper
         );
    ~RosGazeboNode();
   // Constructor; stage itself needs argc/argv.  fname is the .world file
    // that stage should load.
    RosGazeboNode(int argc, char** argv, const char* fname,
         PR2::PR2Robot          *myPR2,
         controller::ArmController          *myArm,
         controller::HeadController         *myHead,
         controller::SpineController        *mySpine,
         controller::BaseController         *myBase,
         controller::LaserScannerController *myLaserScanner,
         controller::GripperController      *myGripper,
         controller::JointController** ControllerArray,
         controller::RosJointController ** RosControllerArray
         );

    // advertise / subscribe models
    int AdvertiseSubscribeMessages();

    // Do one update of the simulator.  May pause if the next update time
    // has not yet arrived.
    void Update();

    // Message callback for a std_msgs::BaseVel message, which set velocities.
    void cmdvelReceived();

    // Message callback for a std_msgs::PR2Arm message, which sets arm configuration.
    void cmd_leftarmconfigReceived();
    void cmd_rightarmconfigReceived();

    void cmd_leftarmcartesianReceived();
    void cmd_rightarmcartesianReceived();

		// Service which sets pr2_kin.q_IK_guess to the current manipulator configuration.
		// July 24, 2008 - Advait - only right arm is supported
    bool reset_IK_guess(rosgazebo::VoidVoid::request &req, rosgazebo::VoidVoid::response &res);
		bool SetRightArmCartesian(rosgazebo::MoveCartesian::request &req, rosgazebo::MoveCartesian::response &res);
		bool OperateRightGripper(rosgazebo::GripperCmd::request &req, rosgazebo::GripperCmd::response &res);

    // arm joint data
    std_msgs::PR2Arm leftarm;
    std_msgs::PR2Arm rightarm;

    // end effector cmds
    pr2_msgs::EndEffectorState cmd_leftarmcartesian;
    pr2_msgs::EndEffectorState cmd_rightarmcartesian;

    std_msgs::Empty empty_msg;

    //Flags to indicate that a new message has arrived
    bool newRightArmPos;
    bool newLeftArmPos;

    //Flag set to indicate that we should use new controls architecture
    bool useControllerArray; 

    //Keep track of controllers
    controller::JointController** ControllerArray;

    typedef list<controller::RosJointController *> RCList;
    RCList RosControllers;
    
    // A small API to expose the joints only even when no controller is present
    typedef list<controller::RosJoint *> RJList;
    RJList RosJoints;
};




