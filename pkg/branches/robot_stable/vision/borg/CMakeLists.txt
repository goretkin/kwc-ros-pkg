cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(borg)
#set(ROS_BUILD_TYPE Release)
add_definitions(-Wall -O3 -g)
rospack_add_library(borg borg.cpp cam_dc1394.cpp cam.cpp stage.cpp)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -pg")

include(FindPkgConfig)
pkg_check_modules(SDL REQUIRED sdl)
include_directories(${SDL_INCLUDE_DIRS})
link_directories(${SDL_LIBRARY_DIRS})

rospack_add_executable(scan scan.cpp)
target_link_libraries(scan borg)

rospack_add_executable(scan_pipeline scan_pipeline.cpp)
target_link_libraries(scan_pipeline borg)

#rospack_add_executable(home home.cpp)
#target_link_libraries(home borg pthread)

#rospack_add_executable(goto goto.cpp)
#target_link_libraries(goto borg pthread)

#rospack_add_executable(duty duty.cpp)
#target_link_libraries(duty borg pthread)

#rospack_add_executable(scan_enc_test scan_enc_test.cpp)
#target_link_libraries(scan_enc_test borg pthread)

rospack_add_executable(view_cloud view_cloud.cpp)
target_link_libraries(view_cloud cloud_viewer)

#rospack_add_executable(extract extract.cpp)
#target_link_libraries(extract borg pthread)

#rospack_add_executable(project project.cpp)
#target_link_libraries(project borg pthread)

#rospack_add_executable(calibrate calibrate.cpp)
#target_link_libraries(calibrate borg pthread)

#rospack_add_executable(view view.cpp)
#target_link_libraries(view borg)
#target_link_libraries(view ${SDL_LIBRARIES})

