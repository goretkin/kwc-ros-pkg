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
#include <VelocityControllers.hh>

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

#include <list>

namespace ros {
  namespace highlevel_controllers {

    class MoveBase : public HighlevelController<std_msgs::Planner2DState, std_msgs::Planner2DGoal> {

    public:
      
      typedef std::vector<std_msgs::Point2DFloat32> footprint_t;
      
      /**
       * @brief Constructor
       */
      MoveBase();

      virtual ~MoveBase();
      
      footprint_t const & getFootprint() const;
      
    protected:

      /**
       * @brief Accessor for the cost map. Use mainly for initialization
       * of specialized map strunture for planning
       */
      const CostMap2D& getCostMap() const {return *costMap_;}

      /**
       * @brief A handler to be over-ridden in the derived class to handle a diff stream from the
       * cost map. This is called on a map update, which means it will be on a separate thread to the main
       * node control loop. The client will obtain the lock, so derived class implementations
       * can assume excusinve access
       */
      virtual void handleMapUpdates(const std::vector<unsigned int>& updates){}

      /**
       * @brief Overwrites the current plan with a new one. Will handle suitable publication
       * @see publishPlan
       */
      void updatePlan(const std::list<std_msgs::Pose2DFloat32>& newPlan);

      /**
       * @brief test the current plan for collisions with obstacles
       */
      bool inCollision() const;
      
      /**
       * @brief projection of the robot outline onto the ground plane.
       */
      footprint_t footprint_;
      
      /**
       * @brief Will process a goal update message.
       */
      virtual void updateGoalMsg();

    private:
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
       * @brief On deactivation we should stop the robot
       */
      virtual void handleDeactivation();

      /**
       * @brief Callbacks for perceiving obstalces
       */
      void baseScanCallback();
      void tiltScanCallback();
      void stereoCloudCallback();

      /**
       * @brief Robot odometry call back
       */
      void odomCallback();

      /**
       * @brief Process point cloud data
       * @todo migrate to costmap
       */
      void processData(const std_msgs::PointCloud& local_cloud);

      void updateGlobalPose();

      /**
       * @brief Helper method to update the costmap and conduct other book-keeping
       */
      void updateDynamicObstacles(double ts, const std_msgs::PointCloud& cloud);

      /**
       * @brief Issue zero velocity commands
       */
      void stopRobot();

      void publishPath(bool isGlobal, const std::list<std_msgs::Pose2DFloat32>& path);

      void publishFootprint(double x, double y, double th);

      /**
       * Utility to publish the local cost map around the robot
       */
      void publishLocalCostMap();

      /**
       * @brief Utility for comparing 2 points to be within a required distance, which is specified as a
       * configuration parameter of the object.
       */
      bool withinDistance(double x1, double y1, double th1, double x2, double y2, double th2) const ;

      /**
       * @brief Test if point in the square footprint of the robot
       */
      bool inFootprint(double x, double y) const;

      /**
       * @brief Provide a filtered set of points based on the extraction of the robot footprint and the 
       * filter based on the min and max z values
       */
      std_msgs::PointCloud * extractFootprintAndGround(const std_msgs::PointCloud& baseFrameCloud) const;

      /**
       * @brief Watchdog Handling
       */
      void petTheWatchDog();

      bool checkWatchDog() const;

      struct timeval lastUpdated_;

      // Callback messages
      std_msgs::LaserScan baseScanMsg_; /**< Filled by subscriber with new base laser scans */
      std_msgs::LaserScan tiltScanMsg_; /**< Filled by subscriber with new tilte laser scans */
      std_msgs::PointCloud stereoCloudMsg_; /**< Filled by subscriber with point clouds */
      std_msgs::RobotBase2DOdom odomMsg_; /**< Odometry in the odom frame picked up by subscription */
      laser_scan::LaserProjection projector_; /**< Used to project laser scans */

      rosTFClient tf_; /**< Used to do transforms */

      /** Should encapsulate as a controller wrapper that is not resident in the trajectory rollout package */
      VelocityController* controller_;

      CostMap2D* costMap_; /**< The cost map mainatined incrementally from laser scans */

      CostMapAccessor* ma_; /**< Sliding read-only window on the cost map */

      libTF::TFPose2D global_pose_; /**< The global pose in the map frame */

      std_msgs::RobotBase2DOdom base_odom_; /**< Odometry in the base frame */

      /** Parameters that will be passed on initialization soon */
      double baseLaserMaxRange_; /**< Used in laser scan projection */
      double tiltLaserMaxRange_; /**< Used in laser scan projection */

      std::list<std_msgs::Pose2DFloat32>  plan_; /**< The 2D plan in grid co-ordinates of the cost map */

      std::deque<std_msgs::PointCloud> point_clouds_; /**< Buffer point clouds until a transform is available */
      ros::thread::mutex bufferMutex_;

      // Filter parameters
      double minZ_;
      double maxZ_;
      double robotWidth_;
    };
  }
}
#endif
