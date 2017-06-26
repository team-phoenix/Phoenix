cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH
	${CMAKE_CURRENT_LIST_DIR}/CMake
	${CMAKE_CURRENT_LIST_DIR}/externals/cmake-modules
	${CMAKE_CURRENT_LIST_DIR}/frontend
)

project(Phoenix)
add_executable(${PROJECT_NAME} MACOSX_BUNDLE "")

##
## Phoenix source
##

include(frontend/CMakeLists.txt)
include(backend/CMakeLists.txt)

##
## Externals
##

find_package(Libsamplerate)
if(LIBSAMPLERATE_FOUND)
	include_directories(${LIBSAMPLERATE_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${LIBSAMPLERATE_LIBRARY})
else()
	add_subdirectory(externals/libsamplerate-0.1.9-cmake-patch)
	target_link_libraries(${PROJECT_NAME} samplerate)
endif()

find_package(ZLIB)
if(ZLIB_FOUND)
	include_directories(${ZLIB_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE})
else()
	add_subdirectory(externals/zlib-1.2.11)
	target_link_libraries(${PROJECT_NAME} zlibstatic)
endif()

find_package(Quazip)
if(QUAZIP_FOUND)
	include_directories(${QUAZIP_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${QUAZIP_LIBRARIES})
else()
	add_definitions(-DQUAZIP_STATIC)
	add_subdirectory(externals/quazip-0.7.3)
	target_link_libraries(${PROJECT_NAME} quazip_static)
endif()

find_package(SDL2)
if(SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_SDLMAIN_LIBRARY})
else()
	set(SDL_SHARED OFF CACHE BOOL "" FORCE)
	set(SDL_STATIC ON CACHE BOOL "" FORCE)
	add_subdirectory(externals/sdl-2.0.5)
	target_link_libraries(${PROJECT_NAME} SDL2-static SDL2main)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Concurrent Core Multimedia Network Qml Quick Sql Svg)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent Qt5::Core Qt5::Multimedia Qt5::Network Qt5::Qml Qt5::Quick Qt5::Sql Qt5::Svg)

# On macOS, we must use QtPlatformHeaders to grab the pointer to the main window's native OpenGL context (NSOpenGLContext)
# http://doc.qt.io/qt-5/qtplatformheaders-index.html
if(APPLE)
	# QtPlatformHeaders is a header-only module, leaving us with no choice but to get its location in this hacky way
	# FIXME: This is not necessary when using qmake, but is when using CMake?
	set(_qt5_platform_headers ${_qt5_root_dir}/../../include)
	get_filename_component(_qt5_platform_headers ${_qt5_platform_headers} ABSOLUTE)
	message("Found QtPlatformHeaders: " ${_qt5_platform_headers})
	target_include_directories(${PROJECT_NAME} PUBLIC ${_qt5_platform_headers})

	# FIXME: For some reason this is necessary on macOS?
	find_library(FOUNDATION Foundation REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${FOUNDATION})
endif()

##
## Compiler
##

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED 11)

##
## Distribution
##

include(CMake/DistCommon.cmake)
