cmake_minimum_required (VERSION 2.6)

### If you're trying to debug include directives or so, this here
### might help:
set (CMAKE_VERBOSE_MAKEFILE ON)

set (ROS_BUILD_TYPE Release)
#set (ROS_BUILD_TYPE Debug)
include (rosbuild)
add_definitions (-Wall)
rospack (sbpl_util)

rospack_add_library (sbpl_util
		     src/sbpl_util.cpp
		     src/costmap_wrap.cpp
		     src/footprint.cpp
		     src/environment_wrap.cpp
		     src/plan_wrap.cpp)
target_link_libraries (sbpl_util costmap_2d)

rospack_add_executable (t3dobst src/t3dobst.cpp)
target_link_libraries (t3dobst sbpl_util costmap_2d sunflower)
