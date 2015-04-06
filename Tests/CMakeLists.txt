add_executable(BuildTest
BuildTest.cpp)

target_link_libraries(BuildTest Perturb)

add_test(Build_Test BuildTest 0)

