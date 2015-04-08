add_executable(BuildTest
BuildTest.cpp)
target_link_libraries(BuildTest Perturb)

add_executable(OutputTest
OutputTest.cpp)
target_link_libraries(OutputTest Perturb)


add_test(Build_Test BuildTest 0)

