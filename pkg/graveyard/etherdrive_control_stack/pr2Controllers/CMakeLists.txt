cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(pr2Controllers)
add_definitions(-Wall)
rospack_add_library(pr2Controllers src/BaseController.cpp src/HeadController.cpp src/SpineController.cpp src/ArmController.cpp src/LaserScannerController.cpp src/GripperController.cpp)
#rospack_add_library(pr2Controllers src/BaseController.cpp)
