cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(topological_map)

#uncomment for profiling
set(ROS_COMPILE_FLAGS "-g -pg -O3" ${ROS_COMPILE_FLAGS})
set(ROS_LINK_FLAGS "-g -pg -O3" ${ROS_LINK_FLAGS})

rospack_add_executable(ros_bottleneck_graph src/ros_bottleneck_graph.cpp src/bottleneck_graph.cpp)
rospack_add_executable(bg_driver src/bg_driver.cpp src/bottleneck_graph.cpp)
