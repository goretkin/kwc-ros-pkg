cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(gazebo_plugin)
genmsg()
include_directories(msg/cpp)
gensrv()
include_directories(srv/cpp)

#add_definitions(-Wall -DNDEBUG -O3)
set(ROS_BUILD_TYPE Release)

rospack_add_library(Ros_Stereo_Camera src/Ros_Stereo_Camera.cc)
rospack_add_library(Ros_Camera src/Ros_Camera.cc)
rospack_add_library(Ros_Laser src/Ros_Laser.cc)
rospack_add_library(Ros_Block_Laser src/Ros_Block_Laser.cc)
rospack_add_library(Ros_Time src/Ros_Time.cc)
rospack_add_library(Ros_PTZ src/Ros_PTZ.cc)
rospack_add_library(P3D src/P3D.cc)
rospack_add_library(F3D src/F3D.cc)
rospack_add_library(gazebo_mechanism_control src/gazebo_mechanism_control.cpp)
rospack_add_library(gazebo_battery src/gazebo_battery.cpp)

# This target requires adding player to the manifest.xml.
#rospack_add_executable(player_pr2 src/player/Pr2_Player.cc)
#target_link_libraries(player_pr2 playerc++)

rospack_add_rostest(test/testslide.xml)
rospack_add_rostest(test/testcameras.xml)
rospack_add_rostest(test/testscan.xml)

