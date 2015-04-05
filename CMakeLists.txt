cmake_minimum_required(VERSION 2.6)
project(PERTURBSDR)
add_definitions(-std=c++11 -pthread -fPIC)
add_definitions(-DTHERON_XS=0 -DTHERON_NUMA=0 -DTHERON_CPP11=1 -DTHERON_BOOST=0 -DTHERON_POSIX=1 -DTHERON_WINDOWS=0 -DNDEBUG)
set(PERTURB_VERSION_MAJOR 0)
set(PERTURB_VERSION_MINOR 2)
set(PERTURB_VERSION_FIX 0)
set(PERTURB_VERSION "${PERTURB_VERSION_MAJOR}.${PERTURB_VERSION_MINOR}.${PERTURB_VERSION_FIX}")
set(BUILD_SHARED_LIBS ON)
set(SOVERSION "${PERTURB_VERSION_MAJOR}.${PERTURB_VERSION_MINOR}")

configure_file (
  "${PROJECT_SOURCE_DIR}/Build-Files/PerturbConfig.h.in"
  "${PROJECT_BINARY_DIR}/Include/PerturbConfig.h"
  )
  
include_directories("${PROJECT_SOURCE_DIR}/Include")
include_directories("${PROJECT_SOURCE_DIR}/ExternalCode/Theron/Include")
include_directories("${PROJECT_SOURCE_DIR}/ExternalCode/Theron/Lib")
include_directories("${PROJECT_BINARY_DIR}")

find_package (Threads)
message(STATUS "Building version: ${PERTURB_VERSION}")

enable_testing()

#add_custom_target(Theron
#COMMAND make 'library' 'shared=on' 'c++11=on' 'mode=release'
#WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/ExternalCode/Theron
#)
#LINK_DIRECTORIES("${PROJECT_SOURCE_DIR}/ExternalCode/Theron/Lib")



add_subdirectory("${PROJECT_SOURCE_DIR}/ExternalCode/Theron")
add_subdirectory("${PROJECT_SOURCE_DIR}/Source")
add_subdirectory("${PROJECT_SOURCE_DIR}/Tests")

