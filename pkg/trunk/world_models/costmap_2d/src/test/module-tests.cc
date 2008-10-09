/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @author Conor McGann
 * @file Test harness for CostMap2D
 */

#include <costmap_2d/costmap_2d.h>
#include <set>
#include <gtest/gtest.h>

using namespace costmap_2d;

const unsigned char MAP_10_BY_10_CHAR[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 200, 200, 200,
  0, 0, 0, 0, 100, 0, 0, 200, 200, 200,
  0, 0, 0, 0, 100, 0, 0, 200, 200, 200,
  70, 70, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 200, 200, 200, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 255, 255, 255,
  0, 0, 0, 0, 0, 0, 0, 255, 255, 255
};

std::vector<unsigned char> MAP_10_BY_10;

const unsigned int GRID_WIDTH(10);
const unsigned int GRID_HEIGHT(10);
const double RESOLUTION(1);
const double WINDOW_LENGTH(10);
const unsigned char THRESHOLD(100);
const double MAX_Z(1.0);
const double ROBOT_RADIUS(1.0);

bool find(const std::vector<unsigned int>& l, unsigned int n){
  for(std::vector<unsigned int>::const_iterator it = l.begin(); it != l.end(); ++it){
    if(*it == n)
      return true;
  }

  return false;
}

TEST(costmap, test1){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD);
  ASSERT_EQ(map.getWidth(), 10);
  ASSERT_EQ(map.getHeight(), 10);

  // Verify that obstacles correctly identified from the static map.
  std::vector<unsigned int> occupiedCells;
  map.getOccupiedCellDataIndexList(occupiedCells);
  ASSERT_EQ(occupiedCells.size(), 14);

  // Iterate over all id's and verify that they are present according to their
  const unsigned char* costData = map.getMap();
  for(std::vector<unsigned int>::const_iterator it = occupiedCells.begin(); it != occupiedCells.end(); ++it){
    unsigned int ind = *it;
    unsigned int x, y;
    map.IND_MC(ind, x, y);
    ASSERT_EQ(find(occupiedCells, map.MC_IND(x, y)), true);
    ASSERT_EQ(MAP_10_BY_10[ind] >= 100, true);
    ASSERT_EQ(costData[ind] >= 100, true);
  }

  // Block of 200
  ASSERT_EQ(find(occupiedCells, map.MC_IND(7, 2)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(8, 2)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(9, 2)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(7, 3)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(8, 3)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(9, 3)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(7, 4)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(8, 4)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(9, 4)), true);

  // Block of 100
  ASSERT_EQ(find(occupiedCells, map.MC_IND(4, 3)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(4, 4)), true);

  // Block of 200
  ASSERT_EQ(find(occupiedCells, map.MC_IND(3, 7)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(4, 7)), true);
  ASSERT_EQ(find(occupiedCells, map.MC_IND(5, 7)), true);


  // Verify Coordinate Transformations, ROW MAJOR ORDER
  ASSERT_EQ(map.WC_IND(0.0, 0.0), 0);
  ASSERT_EQ(map.WC_IND(0.0, 0.99), 0);
  ASSERT_EQ(map.WC_IND(0.0, 1.0), 10);
  ASSERT_EQ(map.WC_IND(1.0, 0.99), 1);
  ASSERT_EQ(map.WC_IND(9.99, 9.99), 99);
  ASSERT_EQ(map.WC_IND(8.2, 3.4), 38);

  // Ensure we hit the middle of the cell for world co-ordinates
  double wx, wy;
  map.IND_WC(99, wx, wy);
  ASSERT_EQ(wx, 9.5);
  ASSERT_EQ(wy, 9.5);
}

/**
 * Verify that static obstacles are not removed by an expired stamp
 */
TEST(costmap, test2){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD);
  std::set<unsigned int> updates;
  map.removeStaleObstacles(1000, updates);
  ASSERT_EQ(updates.size(), 0);

  // Now verify that we still have all our expected obstacles
  std::vector<unsigned int> ids;
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 14);
}

/**
 * Verify that dynamic obstacles are added and removed
 */
TEST(costmap, test3){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD);

  // Add a point cloud and verify its insertion. There should be only one new if and now deletions
  std_msgs::PointCloudFloat32 cloud;
  cloud.set_pts_size(3);
  cloud.pts[0].x = 0;
  cloud.pts[0].y = 0;
  cloud.pts[1].x = 0;
  cloud.pts[1].y = 0;
  cloud.pts[2].x = 0;
  cloud.pts[2].y = 0;

  std::set<unsigned int> updates;
  std::vector<unsigned int> ids;
  map.updateDynamicObstacles(1, cloud, updates);

  // Should now have 1 insertion and no deletions
  ASSERT_EQ(updates.size(), 1);
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 15);

  // Repeating the call for the same time step we should see no insertions or deletions
  map.updateDynamicObstacles(1, cloud, updates);
  ASSERT_EQ(updates.empty(), true);

  // Updating for a window prior to expiration, should see no deletions
  map.removeStaleObstacles(5, updates);
  ASSERT_EQ(updates.empty(), true);
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 15);

  // Update for a window after expiration, should see 1 deletion
  map.removeStaleObstacles(11, updates);
  ASSERT_EQ(updates.size(), 1);
  ASSERT_EQ(map[*(updates.begin())], 0);
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 14);
}

/**
 * Verify that if we add a point that is already a static obstacle we do not end up with a new ostacle
 */
TEST(costmap, test4){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD);

  // A point cloud with one point that falls within an existing obstacle
  std_msgs::PointCloudFloat32 cloud;
  cloud.set_pts_size(1);
  cloud.pts[0].x = 7;
  cloud.pts[0].y = 2;

  std::set<unsigned int> updates;
  std::vector<unsigned int> ids;
  map.updateDynamicObstacles(1, cloud, updates);
  ASSERT_EQ(updates.empty(), true);
}

/**
 * Add an obstacle that is on a cell with a positive cost but less than the threshold for a lethal
 * obstacle. Expect that we get a new obstacle. When the obstacle times out, should revert to remove
 * the obstacle but keep the cost in the full map at the original value.
 */
TEST(costmap, test5){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD);

  std::set<unsigned int> updates;

  // A point cloud with 2 points falling in a cell with a non-lethal cost
  std_msgs::PointCloudFloat32 c0;
  c0.set_pts_size(2);
  c0.pts[0].x = 0;
  c0.pts[0].y = 5;
  c0.pts[1].x = 1;
  c0.pts[1].y = 5;

  map.updateDynamicObstacles(1, c0, updates);

  ASSERT_EQ(updates.size(), 2);
  ASSERT_EQ(map[map.WC_IND(0, 5)], CostMap2D::LETHAL_OBSTACLE);
  ASSERT_EQ(map[map.WC_IND(1, 5)], CostMap2D::LETHAL_OBSTACLE);

  // Pet the watchdog with 1 point only
  std_msgs::PointCloudFloat32 c1;
  c1.set_pts_size(1);
  c1.pts[0].x = 0;
  c1.pts[0].y = 5;
  map.updateDynamicObstacles(WINDOW_LENGTH-1, c1, updates);
  ASSERT_EQ(updates.empty(), true);

  // Update map for later time point. SHould remove one of the dynamic obstacles, reverting to a value less than the threshold
  map.removeStaleObstacles(WINDOW_LENGTH + 1, updates);
  ASSERT_EQ(updates.size(), 1);
  ASSERT_EQ(map[map.WC_IND(0, 5)], CostMap2D::LETHAL_OBSTACLE);
  ASSERT_EQ(map[map.WC_IND(1, 5)] < CostMap2D::LETHAL_OBSTACLE, true);
  ASSERT_EQ(map[map.WC_IND(1, 5)] > 0, true);
  ASSERT_EQ(map[map.WC_IND(1, 5)] == MAP_10_BY_10[map.WC_IND(1, 5)], true);
}

/**
 * Make sure we ignore points outside of our z threshold
 */
TEST(costmap, test6){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD, MAX_Z);

  // A point cloud with 2 points falling in a cell with a non-lethal cost
  std_msgs::PointCloudFloat32 c0;
  c0.set_pts_size(2);
  c0.pts[0].x = 0;
  c0.pts[0].y = 5;
  c0.pts[0].z = 0.4;
  c0.pts[1].x = 1;
  c0.pts[1].y = 5;
  c0.pts[1].z = 1.2;

  std::set<unsigned int> updates;
  map.updateDynamicObstacles(1, c0, updates);
  ASSERT_EQ(updates.size(), 1);
}

/**
 * Test inflation for both static and dynamic obstacles
 */
TEST(costmap, test7){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD, MAX_Z, 
		ROBOT_RADIUS, ROBOT_RADIUS, ROBOT_RADIUS);


  // Verify that obstacles correctly identified
  std::vector<unsigned int> occupiedCells;
  map.getOccupiedCellDataIndexList(occupiedCells);

  std::cout << map.toString();

  // There should be no duplicates
  std::set<unsigned int> setOfCells;
  for(unsigned int i=0;i<occupiedCells.size(); i++)
    setOfCells.insert(i);

  ASSERT_EQ(setOfCells.size(), occupiedCells.size());
  ASSERT_EQ(setOfCells.size(), 47);

  const unsigned char* costData = map.getMap();

  // Iterate over all id's and verify they are obstacles
  for(std::vector<unsigned int>::const_iterator it = occupiedCells.begin(); it != occupiedCells.end(); ++it){
    unsigned int ind = *it;
    unsigned int x, y;
    map.IND_MC(ind, x, y);
    ASSERT_EQ(find(occupiedCells, map.MC_IND(x, y)), true);
    ASSERT_EQ(costData[ind] == CostMap2D::LETHAL_OBSTACLE || costData[ind] == CostMap2D::INSCRIBED_INFLATED_OBSTACLE, true);
  }

  // Set an obstacle at the origin and observe insertions for it and its neighbors
  std::set<unsigned int> updates;
  std_msgs::PointCloudFloat32 c0;
  c0.set_pts_size(1);
  c0.pts[0].x = 0;
  c0.pts[0].y = 0;
  c0.pts[0].z = 0.4;
  map.updateDynamicObstacles(1, c0, updates);

  // It and its 3 neighbors makes 4 obstacles
  ASSERT_EQ(updates.size(), 4);

  // Add an obstacle at <2,0> which will inflate and refresh to of the other inflated cells
  std_msgs::PointCloudFloat32 c1;
  c1.set_pts_size(1);
  c1.pts[0].x = 2;
  c1.pts[0].y = 0;
  c1.pts[0].z = 0.0;
  map.updateDynamicObstacles(WINDOW_LENGTH - 1, c1, updates);

  // Now we expect insertions for it, and 3 more neighbors, but not all 5
  ASSERT_EQ(updates.size(), 4);

  // Now verify that some of the initial batch go away but others remain
  updates.clear();
  map.removeStaleObstacles(WINDOW_LENGTH + 1, updates);
  ASSERT_EQ(updates.size(), 2);

  // Add an obstacle at <1, 9>. This will inflate obstacles around it
  std_msgs::PointCloudFloat32 c2;
  c2.set_pts_size(1);
  c2.pts[0].x = 1;
  c2.pts[0].y = 9;
  c2.pts[0].z = 0.0;
  map.updateDynamicObstacles(WINDOW_LENGTH + 2, c2, updates);
  ASSERT_EQ(map.getCost(1, 9), CostMap2D::LETHAL_OBSTACLE);
  ASSERT_EQ(map.getCost(0, 9), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);
  ASSERT_EQ(map.getCost(2, 9), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);
  ASSERT_EQ(map.getCost(2, 8), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);
  ASSERT_EQ(map.getCost(0, 8), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);

  // Add an obstacle and verify that it over-writes its inflated status
  std_msgs::PointCloudFloat32 c3;
  c3.set_pts_size(1);
  c3.pts[0].x = 0;
  c3.pts[0].y = 9;
  c3.pts[0].z = 0.0;
  map.updateDynamicObstacles(WINDOW_LENGTH + 3, c3, updates);
  ASSERT_EQ(map.getCost(0, 9), CostMap2D::LETHAL_OBSTACLE);
}

/**
 * Test specific inflation scenario to ensure we do not set inflated obstacles to be raw obstacles.
 */
TEST(costmap, test8){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD, MAX_Z, 
		ROBOT_RADIUS, ROBOT_RADIUS, ROBOT_RADIUS);

  std::set<unsigned int> updates;

  // Creat a small L-Shape all at once
  std_msgs::PointCloudFloat32 c0;
  c0.set_pts_size(3);
  c0.pts[0].x = 1;
  c0.pts[0].y = 1;
  c0.pts[0].z = 0;
  c0.pts[1].x = 1;
  c0.pts[1].y = 2;
  c0.pts[1].z = 0;
  c0.pts[2].x = 2;
  c0.pts[2].y = 2;
  c0.pts[2].z = 0;

  map.updateDynamicObstacles(1, c0, updates);
  ASSERT_EQ(map.getCost(3, 2), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);  
  ASSERT_EQ(map.getCost(3, 3), CostMap2D::INSCRIBED_INFLATED_OBSTACLE);
}

/**
 * Test inflation behavior, starting with an empty map
 */
TEST(costmap, test9){
  std::vector<unsigned char> mapData;
  for(unsigned int i=0; i< GRID_WIDTH; i++){
    for(unsigned int j = 0; j < GRID_HEIGHT; j++){
      mapData.push_back(0);
    }
  }

  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, mapData, RESOLUTION, WINDOW_LENGTH, THRESHOLD, MAX_Z, 
		ROBOT_RADIUS * 3, ROBOT_RADIUS * 2, ROBOT_RADIUS);

  // There should be no occupied cells
  std::vector<unsigned int> ids;
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 0);

  // Add an obstacle at 5,5
  std_msgs::PointCloudFloat32 c0;
  c0.set_pts_size(1);
  c0.pts[0].x = 5;
  c0.pts[0].y = 5;
  c0.pts[0].z = 0;

  std::set<unsigned int> updates;
  map.updateDynamicObstacles(1, c0, updates);

  // A cell radius of 3 gives a 7*7 inflation grid
  ASSERT_EQ(updates.size(), 49);

  // The occupied cells are the obstacles and the inscribed inflated obstacles which is given by a 3 * 3 grid
  map.getOccupiedCellDataIndexList(ids);
  ASSERT_EQ(ids.size(), 9);

  // Update again - should see no change
  map.updateDynamicObstacles(2, c0, updates);
  ASSERT_EQ(updates.size(), 0);

  // All the obstacles should go away when we remove stale obstacles
  updates.clear();
  map.removeStaleObstacles(WINDOW_LENGTH + 2, updates);
  ASSERT_EQ(updates.size(), 49);
}

/**
 * Test for the cost map accessor
 */
TEST(costmap, test10){
  CostMap2D map(GRID_WIDTH, GRID_HEIGHT, MAP_10_BY_10, RESOLUTION, WINDOW_LENGTH, THRESHOLD, MAX_Z, ROBOT_RADIUS);

  // A window around a robot in the top left
  CostMapAccessor ma(map, 5, 0, 0);
  double wx, wy;

  // Origin
  ma.MC_WC(0, 0, wx, wy);
  ASSERT_EQ(wx, 0.5);
  ASSERT_EQ(wy, 0.5);


  // Max in x and y
  ma.updateForRobotPosition(9.5, 9.5);
  ma.MC_WC(0, 0, wx, wy);
  ASSERT_EQ(wx, 5.5);
  ASSERT_EQ(wy, 5.5);

  // Off the map in x - assume it ignores the change
  ma.updateForRobotPosition(10.5, 9.5);
  ma.MC_WC(0, 0, wx, wy);
  ASSERT_EQ(wx, 5.5);
  ASSERT_EQ(wy, 5.5);

  // Off the map in y - assume it ignores the change
  ma.updateForRobotPosition(9.5, 10.5);
  ma.MC_WC(0, 0, wx, wy);
  ASSERT_EQ(wx, 5.5);
  ASSERT_EQ(wy, 5.5);


}
int main(int argc, char** argv){
  for(unsigned int i = 0; i< GRID_WIDTH * GRID_HEIGHT; i++)
    MAP_10_BY_10.push_back(MAP_10_BY_10_CHAR[i]);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
