cmake_minimum_required(VERSION 2.6)
project(Perturb-SDR)
set(Perturb-SDR-VERSION_MAJOR 0)
set(Perturb-SDR-VERSION_MINOR 2)
set(Perturb-SDR-VERSION_FIX 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/Build/PerturbConfig.h.in"
  "${PROJECT_BINARY_DIR}/Include/PerturbConfig.h"
  )
  
include_directories("${PROJECT_SOURCE_DIR}/Include")
include_directories("${PROJECT_SOURCE_DIR}/ExternalInclude")



