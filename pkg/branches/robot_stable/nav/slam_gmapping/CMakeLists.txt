cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
include(rosbuild)
set(ROS_BUILD_TYPE Release)
rospack(slam_gmapping)
rospack_add_executable(slam_gmapping src/slam_gmapping.cpp src/main.cpp)

rospack_add_executable(tftest src/tftest.cpp)
