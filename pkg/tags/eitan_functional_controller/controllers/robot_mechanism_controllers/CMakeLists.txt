cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(robot_mechanism_controllers)
genmsg()
gensrv()
rospack_add_library(robot_mechanism_controllers
 src/joint_effort_controller.cpp
 src/joint_position_controller.cpp
 src/joint_velocity_controller.cpp
 src/cartesian_effort_controller.cpp
 src/cartesian_position_controller.cpp
 src/sine_sweep_controller.cpp
 src/joint_autotuner.cpp
 src/joint_pd_controller.cpp
 src/joint_calibration_controller.cpp
 src/joint_blind_calibration_controller.cpp
 src/joint_manual_calibration_controller.cpp)
