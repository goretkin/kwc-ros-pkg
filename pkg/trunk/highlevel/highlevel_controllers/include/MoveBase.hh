/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#ifndef HIGHLEVEL_CONTROLLERS_MOVE_BASE_H
#define HIGHLEVEL_CONTROLLERS_MOVE_BASE_H

#include <HighlevelController.hh>

// Costmap used for the map representation
#include <costmap_2d/costmap_2d.h>

// Message structures used
#include <std_msgs/Planner2DState.h>
#include <std_msgs/Planner2DGoal.h>
#include <std_msgs/LaserScan.h>
#include <std_msgs/BaseVel.h>
#include <std_msgs/RobotBase2DOdom.h>

// For transform support
#include <rosTF/rosTF.h>

// Laser projection
#include "laser_scan_utils/laser_scan.h"

// For Controller components
#include <trajectory_rollout/obstacle_map_accessor.h>
#include <trajectory_rollout/helmsman.h>

namespace ros {
  namespace highlevel_controllers {

    /**
     * @brief Encapsualtion point to allow different algorithms to be used to compute velocity commands in following a path
     */
    class VelocityController {
    public:
      virtual ~VelocityController(){}

      /**
       * @brief Initialize with transform client
       */
      virtual void initialize(rosTFClient& tf){}

      /**
       * @brief Compute velocities for x, y and theta based on an obstacle map, and a current path
       */
      virtual bool computeVelocityCommands(const CostMap2D& costMap, 
					   const vector<std_msgs::Pose2DFloat32>& globalPlan,
					   const libTF::TFPose2D& pose,
					   const std_msgs::BaseVel& currentVel, 
					   std_msgs::BaseVel& cmdVel,
					   vector<std_msgs::Pose2DFloat32>& localPlan) = 0;
    };

    /** 
     * @brief For now stick an implementation in here
     */
    class TrajectoryRolloutController: public VelocityController {
    public:
      TrajectoryRolloutController(double mapDeltaX, double mapDeltaY,
				  double sim_time, int sim_steps, int samples_per_dim,
				  double robot_front_radius, double robot_side_radius, double max_occ_dist, 
				  double pdist_scale, double gdist_scale, double dfast_scale, double occdist_scale, 
				  double acc_lim_x, double acc_lim_y, double acc_lim_th);

      virtual ~TrajectoryRolloutController();

      virtual void initialize(rosTFClient& tf);

      virtual bool computeVelocityCommands(const CostMap2D& costMap, 
					   const vector<std_msgs::Pose2DFloat32>& globalPlan, 
					   const libTF::TFPose2D& pose, 
					   const std_msgs::BaseVel& currentVel, 
					   std_msgs::BaseVel& cmdVel,
					   vector<std_msgs::Pose2DFloat32>& localPlan);

    private:
      const double mapDeltaX_;
      const double mapDeltaY_;
      const double sim_time_;
      const int sim_steps_;
      const int samples_per_dim_;	  
      const double robot_front_radius_; 
      const double robot_side_radius_; 
      const double max_occ_dist_; 	  
      const double pdist_scale_; 
      const double gdist_scale_; 
      const double dfast_scale_; 
      const double occdist_scale_; 	  
      const double acc_lim_x_; 
      const double acc_lim_y_; 
      const double acc_lim_th_;
      Helmsman* helmsman_;
    };

    class MoveBase : public HighlevelController<std_msgs::Planner2DState, std_msgs::Planner2DGoal> {

    public:

      /**
       * @brief Constructor
       */
      MoveBase(VelocityController& vc, double windowLength, unsigned char lethalObstacleThreshold, unsigned char noInformation, double maxZ, double inflationRadius);

      virtual ~MoveBase();

    protected:

      /**
       * @brief Accessor for the cost map. Use mainly for initialization
       * of specialized map strunture for planning
       */
      const CostMap2D& getCostMap() const {return *costMap_;}

      /**
       * @brief A handler to be over-ridden in the derived class to handle a diff stream from the
       * cost map. This is called on a map update, which means it will be on a separate thread to the main
       * node control loop
       */
      virtual void handleMapUpdates(const std::vector<unsigned int>& insertions, std::vector<unsigned int>& deletions){}

      /**
       * @brief Overwrites the current plan with a new one. Will handle suitable publication
       * @see publishPlan
       */
      void updatePlan(const std::vector<std_msgs::Pose2DFloat32>& newPlan);

    private:
      /**
       * @brief Will process a goal update message.
       */
      virtual void updateGoalMsg();

      /**
       * @brief Use global pose to publish currrent state data at the start of each cycle
       */
      virtual void updateStateMsg();

      /**
       * @brief Evaluate if final goal x, y, th has been reached.
       */
      virtual bool goalReached();

      /**
       * @brief Send velocity commands based on local plan. Should check for consistency of
       * local plan.
       */
      virtual bool dispatchCommands();

      /**
       * @brief Call back for handling new laser scans
       */
      void laserScanCallback();

      /**
       * @brief Robot odometry call back
       */
      void odomCallback();

      void updateGlobalPose();

      void publishPath(bool isGlobal, const std::vector<std_msgs::Pose2DFloat32>& path);

      /**
       * @brief Utility for comparing 2 points to be within a required distance, which is specified as a
       * configuration parameter of the object.
       */
      bool withinDistance(double x1, double y1, double th1, double x2, double y2, double th2) const ;

      /** Should encapsulate as a controller wrapper that is not resident in the trajectory rollout package */
      VelocityController& controller_;

      std_msgs::LaserScan laserScanMsg_; /**< Filled by subscriber with new laser scans */

      std_msgs::RobotBase2DOdom odomMsg_; /**< Odometry in the odom frame picked up by subscription */

      laser_scan::LaserProjection projector_; /**< Used to project laser scans */

      rosTFClient tf_; /**< Used to do transforms */

      CostMap2D* costMap_; /**< The cost map mainatined incrementally from laser scans */

      libTF::TFPose2D global_pose_; /**< The global pose in the map frame */

      std_msgs::RobotBase2DOdom base_odom_; /**< Odometry in the base frame */

      /** Parameters that will be passed on initialization soon */
      const double laserMaxRange_; /**< Used in laser scan projection */

      std::vector<std_msgs::Pose2DFloat32>  plan_; /**< The 2D plan in grid co-ordinates of the cost map */
      unsigned int currentWaypointIndex_; /**!< Position in the plan under execution. The current working waypoint */
    };
  }
}
#endif
