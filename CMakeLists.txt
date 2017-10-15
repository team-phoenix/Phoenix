cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH
	${CMAKE_CURRENT_LIST_DIR}/CMake
	${CMAKE_CURRENT_LIST_DIR}/externals/cmake-modules
	${CMAKE_CURRENT_LIST_DIR}/frontend
)

# A fix for Windows
if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
endif()

##
## Phoenix executable
##

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

find_package(Qt5 REQUIRED COMPONENTS Concurrent Core Multimedia Network Qml Quick Sql Svg)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent Qt5::Core Qt5::Multimedia Qt5::Network Qt5::Qml Qt5::Quick Qt5::Sql Qt5::Svg)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON AUTORCC ON)

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

find_package(libsamplerate REQUIRED)
target_link_libraries(Phoenix SRC::samplerate)

find_package(ZLIB REQUIRED)
target_link_libraries(Phoenix ZLIB::ZLIB)

find_package(KF5Archive REQUIRED)
target_link_libraries(Phoenix KF5::Archive)

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2 SDL2::SDL2main)
if(WIN32)
	target_link_libraries(${PROJECT_NAME} user32 gdi32 winmm imm32 ole32 oleaut32 version uuid)
endif()

##
## Compiler
##

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED 11)

##
## Distribution
##

include(CMake/DistCommon.cmake)
