cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(laser_scan_utils)
rospack_add_library(laser_scan src/laser_scan.cc )