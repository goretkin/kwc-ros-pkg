cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(erratic_player)
rospack_add_executable(erratic_player erratic_player.cc)
target_link_libraries(erratic_player playerdrivers)
