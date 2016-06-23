cmake_minimum_required( VERSION 2.8 )
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_CURRENT_SOURCE_DIR}/frontend )

project( Phoenix )

##
## Frontend
##

include( frontend/CMakeLists.txt )

##
## Backend
##

include( backend/CMakeLists.txt )

##
## Qt
##

find_package( Qt5Concurrent REQUIRED )
find_package( Qt5Core REQUIRED )
find_package( Qt5Multimedia REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Qml REQUIRED )
find_package( Qt5Quick REQUIRED )
find_package( Qt5Sql REQUIRED )
find_package( Qt5Svg REQUIRED )

set( CMAKE_INCLUDE_CURRENT_DIR ON )
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )

set( QT_LIBRARIES Qt5::Concurrent Qt5::Core Qt5::Multimedia Qt5::Network Qt5::Qml Qt5::Quick Qt5::Sql Qt5::Svg )

##
## Externals
##

find_package( quazip REQUIRED )
find_package( SDL2 REQUIRED )
find_package( samplerate REQUIRED )
find_package( zlib REQUIRED )

include_directories( ${QUAZIP_INCLUDE_DIRS} )
include_directories( ${SAMPLERATE_INCLUDE_DIR} )
include_directories( ${SDL2_INCLUDE_DIRS} )

list( APPEND EXTERNAL_LIBRARIES quazip SDL2 samplerate z )

# FIXME: For some reason this is necessary on OS X?
if( APPLE )
    find_library( FOUNDATION Foundation )
    list( APPEND EXTERNAL_LIBRARIES ${FOUNDATION} )
endif()

##
## Deployment
##

file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/PHOENIX_PORTABLE "" )
#set( OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/frontend/phoenix.icns )
#set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

##
## Compiler
##

set( CMAKE_CXX_STANDARD 11 )

# Find and include all folders that contain .h files in the entire project
file( GLOB_RECURSE inc_files *.h )
set( DIR_LIST "" )

foreach( inc_file ${inc_files} )
    get_filename_component( inc_path ${inc_file} PATH )
    list( APPEND DIR_LIST ${inc_path} )
endforeach()

list( REMOVE_DUPLICATES DIR_LIST )
include_directories( ${DIR_LIST} )

# For debugging
#foreach( inc_path ${DIR_LIST} )
#    message( ${inc_path} )
#endforeach()

##
## Linker stuff
##

add_executable(
    Phoenix
    #MACOSX_BUNDLE ${OSX_ICON_FILES}
    ${BACKEND_QRC_LIST}
    ${FRONTEND_CONTENT}
    ${SRC_LIST}
)

target_link_libraries( ${PROJECT_NAME} ${EXTERNAL_LIBRARIES} ${QT_LIBRARIES} )
