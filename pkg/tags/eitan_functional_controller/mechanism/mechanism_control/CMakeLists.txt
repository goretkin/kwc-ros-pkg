cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(mechanism_control)
genmsg()
gensrv()
rospack_add_library(mechanism_control src/mechanism_control.cpp)
#rospack_add_executable(ms_publisher_test test/ms_publisher_test.cpp)
#rospack_add_rostest(test/test-mechanism-state-cpp.xml)
#rospack_add_rostest(test/test-mechanism-state-py.xml)
