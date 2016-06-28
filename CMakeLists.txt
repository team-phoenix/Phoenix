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

find_package( GStreamer REQUIRED COMPONENTS
	gstreamer-app gstreamer-audio gstreamer-fft gstreamer-gl
	gstreamer-mpegts gstreamer-pbutils gstreamer-tag gstreamer-video )
target_link_libraries( ${PROJECT_NAME}
	${GSTREAMER_LIBRARIES} ${GSTREAMER_APP_LIBRARIES} ${GSTREAMER_AUDIO_LIBRARIES}
	${GSTREAMER_FFT_LIBRARIES} ${GSTREAMER_GL_LIBRARIES} ${GSTREAMER_MPEGTS_LIBRARIES}
	${GSTREAMER_PBUTILS_LIBRARIES} ${GSTREAMER_TAG_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARIES} )
target_include_directories( ${PROJECT_NAME}
	PUBLIC
	${GSTREAMER_INCLUDE_DIRS} ${GSTREAMER_APP_INCLUDE_DIRS}	${GSTREAMER_AUDIO_INCLUDE_DIRS}
	${GSTREAMER_FFT_INCLUDE_DIRS} ${GSTREAMER_GL_INCLUDE_DIRS} ${GSTREAMER_MPEGTS_INCLUDE_DIRS}
	${GSTREAMER_PBUTILS_INCLUDE_DIRS} ${GSTREAMER_TAG_INCLUDE_DIRS} ${GSTREAMER_VIDEO_INCLUDE_DIRS} )

find_package( GLIB REQUIRED COMPONENTS gio gobject gmodule gthread )
target_link_libraries( ${PROJECT_NAME} ${GLIB_LIBRARIES} ${GLIB_GIO_LIBRARIES}
	${GLIB_GOBJECT_LIBRARIES} ${GLIB_GMODULE_LIBRARIES} ${GLIB_GTHREAD_LIBRARIES} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${GLIB_INCLUDE_DIRS} )

find_package( LibXml2 REQUIRED )
target_link_libraries( ${PROJECT_NAME} ${LIBXML2_LIBRARIES} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${LIBXML2_INCLUDE_DIR} )

find_package( Intl REQUIRED )
target_link_libraries( ${PROJECT_NAME} ${Intl_LIBRARIES} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${Intl_INCLUDE_DIRS} )

find_package( Iconv REQUIRED )
target_link_libraries( ${PROJECT_NAME} ${ICONV_LIBRARIES} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${ICONV_INCLUDE_DIRS} )

##
## Compiler
##

set_target_properties( ${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED 11 )

##
## Distribution
##

include( CMake/DistCommon.cmake )
