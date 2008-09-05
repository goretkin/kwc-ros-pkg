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
#ifndef TRAJECTORY_CONTROLLER_H_
#define TRAJECTORY_CONTROLLER_H_

#include <vector>
#include <iostream>
#include <math.h>
#include <utility>

#include "map_cell.h"
#include "map_grid.h"
#include "trajectory.h"

#define SIM_STEPS 20
#define SIM_TIME 2 //in seconds

#define MAX(x, y) x > y ? x : y
#define MIN(x, y) x < y ? x : y
#define VALID_CELL(map, i, j) ((i >= 0) && (i < (int)map.rows_) && (j >= 0) && (j < (int)map.cols_))

//Based on the plan from the path planner, determine what velocities to send to the robot
class TrajectoryController {
  public:
    //create a controller given a map, path, and acceleration limits of the robot
    TrajectoryController(MapGrid mg, std::vector<std::pair<int, int> > path, double acc_x, double acc_y, double acc_theta);

    //compute the distance from each cell in the map grid to the planned path
    void computePathDistance();
    
    //compute the distance from an individual cell to the planned path
    void cellPathDistance(MapCell* current, int di, int dj);

    //update neighboring path distance
    void updateNeighbors(MapCell* current);

    //create a trajectory given the current pose of the robot and selected velocities
    void generateTrajectory(double x, double y, double theta, double vx, double vy, 
        double vtheta, double num_steps, double sim_time);

    //compute position based on velocity
    double computeNewPosition(double xi, double v, double dt);

    //compute velocity based on acceleration
    double computeNewVelocity(double vg, double vi, double amax, double dt);
    
    //the map passed on from the planner
    MapGrid map_;

    //the path computed by the planner
    std::vector<std::pair<int, int> > planned_path_;

    //the acceleration limits of the robot
    double acc_x_, acc_y_, acc_theta_;
    
};


#endif
