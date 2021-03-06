cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(gazebo_plugin)
genmsg()
include_directories(msg/cpp)
gensrv()
include_directories(srv/cpp)

add_definitions(-fPIC)
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
rospack_add_library(Ros_Bumper src/Ros_Bumper.cc)

# This target requires adding player to the manifest.xml.
#rospack_add_executable(player_pr2 src/player/Pr2_Player.cc)
#target_link_libraries(player_pr2 playerc++)

rospack_add_rostest(test/test_slide.xml)
rospack_add_rostest(test/test_base.xml)
rospack_add_rostest(test/test_arm.xml)
rospack_add_rostest(test/test_pendulum.xml)
rospack_add_rostest(test/test_scan.xml)

rospack_add_rostest(test/hztest_pr2.xml)
rospack_add_rostest(test/hztest_rostime.xml)
rospack_add_rostest(test/hztest_pr2_odom.xml)

# below need xvfb-run
# test_camera seems to fail a lot
rospack_add_rostest_future(test/test_camera.xml)
rospack_add_rostest_graphical(test/hztest_pr2_image.xml)

rospack_add_executable(urdf2factory src/urdf2factory.cc)
#rospack_add_executable(simiface src/simiface.cc)
