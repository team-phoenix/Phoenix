##
## Source files
##

include( ${CMAKE_CURRENT_LIST_DIR}/consumer/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/control/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/core/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/input/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/pipeline/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/util/CMakeLists.txt )

target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/backendplugin.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/backendcommon.h
	${CMAKE_CURRENT_LIST_DIR}/backendplugin.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
