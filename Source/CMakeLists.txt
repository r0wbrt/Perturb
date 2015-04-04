set( PERTURB_LIB_SRC
Application.cpp
Part.cpp
PartFactory.cpp
PartInterface.cpp
)

set(PERTURB_PUBLIC_HEADERS
${PROJECT_SOURCE_DIR}/Include/Application.h
${PROJECT_SOURCE_DIR}/Include/Part.h
${PROJECT_SOURCE_DIR}/Include/PartFactory.h
${PROJECT_SOURCE_DIR}/Include/PartInterface.h
${PROJECT_SOURCE_DIR}/Include/PartMessages.h
${PROJECT_SOURCE_DIR}/Include/Perturb.h
)

add_library(perturb SHARED ${PERTURB_LIB_SRC})
target_link_libraries(perturb ${CMAKE_REQUIRED_LIBRARIES})
set_target_properties(perturb PROPERTIES
SOVERSION ${SOVERSION}
#PUBLIC_HEADER  "${PERTURB_PUBLIC_HEADERS}"
)
add_dependencies(perturb Theron)
