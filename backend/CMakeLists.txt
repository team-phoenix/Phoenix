##
## Source files
##

include(${CMAKE_CURRENT_LIST_DIR}/consumer/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/control/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/core/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/input/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/pipeline/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/util/CMakeLists.txt)

set(SOURCES_PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/backendplugin.cpp
)

set(SOURCES_PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/backendplugin.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
