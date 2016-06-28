cmake_minimum_required( VERSION 3.6.0 FATAL_ERROR )
list( APPEND CMAKE_MODULE_PATH
	${CMAKE_CURRENT_LIST_DIR}/CMake
	${CMAKE_CURRENT_LIST_DIR}/externals/cmake-modules
	${CMAKE_CURRENT_LIST_DIR}/frontend
)

project( Phoenix )
add_executable( ${PROJECT_NAME} "" )

##
## Phoenix source
##

include( frontend/CMakeLists.txt )
include( backend/CMakeLists.txt )

##
## Externals
##

find_package( Libsamplerate REQUIRED )
if( LIBSAMPLERATE_FOUND )
	include_directories( ${LIBSAMPLERATE_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${LIBSAMPLERATE_LIBRARY} )
endif()

find_package( Quazip REQUIRED )
if( QUAZIP_FOUND )
	include_directories( ${QUAZIP_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${QUAZIP_LIBRARIES} )
endif()

find_package( SDL2 REQUIRED )
if( SDL2_FOUND )
	include_directories( ${SDL2_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY} )
endif()

find_package( ZLIB REQUIRED )
if( ZLIB_FOUND )
	include_directories( ${ZLIB_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE} )
endif()

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set_target_properties( ${PROJECT_NAME} PROPERTIES AUTOMOC ON AUTORCC ON )

find_package( Qt5 REQUIRED COMPONENTS Concurrent Core Multimedia Network Qml Quick Sql Svg )
target_link_libraries( ${PROJECT_NAME} Qt5::Concurrent Qt5::Core Qt5::Multimedia Qt5::Network Qt5::Qml Qt5::Quick Qt5::Sql Qt5::Svg )

if( APPLE )
	find_package( Qt5PlatformSupport REQUIRED )
	include_directories( ${Qt5PlatformSupport_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${Qt5PlatformSupport_LIBRARY} )

	# FIXME: For some reason this is necessary on OS X?
	find_library( FOUNDATION Foundation REQUIRED )
	target_link_libraries( ${PROJECT_NAME} ${FOUNDATION} )
endif()

##
## Compiler
##

set_target_properties( ${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED 11 )

##
## Distribution
##

include( CMake/DistCommon.cmake )
