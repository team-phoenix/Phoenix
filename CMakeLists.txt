cmake_minimum_required( VERSION 2.8 )
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/externals/cmake/modules ${CMAKE_CURRENT_SOURCE_DIR}/frontend )

project( Phoenix )

##
## Misc
##

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
## Frontend
##

include( frontend/CMakeLists.txt )

##
## Backend
##

include( backend/CMakeLists.txt )

##
## Externals
##

find_package( libsamplerate REQUIRED )
find_package( quazip REQUIRED )
find_package( SDL2 REQUIRED )
find_package( zlib REQUIRED )

include_directories( ${LIBSAMPLERATE_INCLUDE_DIR} )
include_directories( ${QUAZIP_INCLUDE_DIR} )
include_directories( ${SDL2_INCLUDE_DIR} )
include_directories( ${ZLIB_INCLUDE_DIR} )

list( APPEND EXTERNAL_LIBRARIES
    ${LIBSAMPLERATE_LIBRARY}
    ${QUAZIP_LIBRARIES}
    ${SDL2_LIBRARY}
    ${SDL2_SDLMAIN_LIBRARY}
    ${ZLIB_LIBRARY_RELEASE}
)

# FIXME: For some reason this is necessary on OS X?
if( APPLE )
    find_library( FOUNDATION Foundation )
    list( APPEND EXTERNAL_LIBRARIES ${FOUNDATION} )
endif()

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
## Compiler
##

set( CMAKE_CXX_STANDARD 11 )

##
## App metadata (icons, version, etc.)
##

# TODO: Put in a better place
# TODO: Portable/installed mode
file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/PHOENIX-PORTABLE "" )

# Embed git tag in file
set( PHOENIX_VER_TUPLE 0.0.1 )
string( REPLACE "." "," PHOENIX_VER_TUPLE ${PHOENIX_VER_TUPLE} )
set( PHOENIX_VER_STR "0.0.1-pre-alpha" )
configure_file( version.h.in version.h @ONLY )

# Windows
set( WIN_RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/frontend/phoenix.rc )
list( APPEND WIN_DEPLOYMENT ${WIN_RC_FILE} )

# OS X
set( OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/frontend/phoenix.icns )
set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
list( APPEND OSX_DEPLOYMENT ${OSX_ICON_FILES} )

##
## Linker stuff
##

add_executable(
    Phoenix
    ${WIN_DEPLOYMENT}
    #MACOSX_BUNDLE ${OSX_DEPLOYMENT}
    ${BACKEND_QRC_LIST}
    ${FRONTEND_CONTENT}
    ${SRC_LIST}
)

target_link_libraries( Phoenix ${EXTERNAL_LIBRARIES} ${QT_LIBRARIES} )
