add_executable(BuildTest
BuildTest.cpp)
target_link_libraries(BuildTest theron)
target_link_libraries (BuildTest ${CMAKE_THREAD_LIBS_INIT})


add_test(Build_Test BuildTest 0)

