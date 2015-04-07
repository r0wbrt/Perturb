set( PERTURB_LIB_SRC
Application.cpp
Part.cpp
#PartFactory.cpp
PartInterface.cpp
)

set(PERTURB_PUBLIC_HEADERS
${PROJECT_SOURCE_DIR}/Include/Application.h
${PROJECT_SOURCE_DIR}/Include/Part.h
#${PROJECT_SOURCE_DIR}/Include/PartFactory.h
${PROJECT_SOURCE_DIR}/Include/PartInterface.h
${PROJECT_SOURCE_DIR}/Include/PartMessages.h
${PROJECT_SOURCE_DIR}/Include/Perturb.h
)

add_library(Perturb_srcs OBJECT ${PERTURB_LIB_SRC})
add_library(Perturb SHARED $<TARGET_OBJECTS:Perturb_srcs> $<TARGET_OBJECTS:Theron>)
target_link_libraries(Perturb ${CMAKE_REQUIRED_LIBRARIES})
target_link_libraries(Perturb ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(Perturb PROPERTIES
SOVERSION ${SOVERSION}
#PUBLIC_HEADER  "${PERTURB_PUBLIC_HEADERS}"
)
add_dependencies(Perturb Theron)
