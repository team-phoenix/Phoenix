option( PORTABLE_MODE "Store all data in the same folder as the binary" ON )

if( PORTABLE_MODE )
	file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/PHOENIX-PORTABLE "" )
endif()

# Embed git tag in file
include( externals/cmake-modules/GetGitRevisionDescription.cmake REQUIRED )
git_describe( PHOENIX_VER_STR )
string( SUBSTRING ${PHOENIX_VER_STR} 1 -1 PHOENIX_VER_TUPLE )
string( REGEX REPLACE "-[0-9a-z\\-]*" "" PHOENIX_VER_TUPLE ${PHOENIX_VER_TUPLE} )
string( REPLACE "." "," PHOENIX_VER_TUPLE ${PHOENIX_VER_TUPLE} )
configure_file( version.h.in version.h @ONLY )

if( WIN32 )
	include( CMake/DistWindows.cmake REQUIRED )
elseif( APPLE )
	include( CMake/DistOSX.cmake REQUIRED )
else()
	include( CMake/DistLinux.cmake REQUIRED )
endif()
