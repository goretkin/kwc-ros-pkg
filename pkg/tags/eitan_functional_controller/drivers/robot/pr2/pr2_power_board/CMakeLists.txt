cmake_minimum_required(VERSION 2.6)
include(rosbuild)
rospack(pr2_power_board)

gensrv()

rospack_add_executable(power power.cpp)
rospack_add_executable(power_node power_node.cpp)
#target_link_libraries(simpletest tf)
#rospack_add_executable(testtf src/test/testtf.cc)
#target_link_libraries(testtf TF)
#rospack_add_executable(test_interp src/test/test_interp.cc)
#target_link_libraries(test_interp TF)
#rospack_add_executable(testMatrix src/test/testMatrix.cc)
#target_link_libraries(testMatrix TF)
#rospack_add_executable(testMatrixQuaternion src/test/testMatrixQuaternion.cc)
#target_link_libraries(testMatrixQuaternion TF)
#rospack_add_gtest(pose3d_unittest test/pose3d_unittest.cpp src/Pose3D.cpp)
#target_link_libraries(pose3d_unittest TF)
#rospack_add_gtest(tf_unittest test/tf_unittest.cpp)
#target_link_libraries(tf_unittest tf)
#rospack_add_gtest(cache_unittest test/cache_unittest.cpp)
#target_link_libraries(cache_unittest tf)
#rospack_add_gtest(bullet_unittest test/bullet_unittest.cpp)
#target_link_libraries(bullet_unittest tf)


#rospack_add_executable(btTest test/quaternion.cpp)
#rospack_add_executable(testClient test/testClient.cpp)
#target_link_libraries(testClient tf)
                                 
#rospack_add_executable(testSender test/testSender.cpp)
#target_link_libraries(testSender tf)
