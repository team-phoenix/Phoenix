##
## Source files
##

include( ${CMAKE_CURRENT_LIST_DIR}/cpp/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/cpp/library/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/cpp/library/database/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/cpp/library/fileinfo/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/cpp/library/model/CMakeLists.txt )
include( ${CMAKE_CURRENT_LIST_DIR}/cpp/library/scanner/CMakeLists.txt )

##
## QML
##

file( GLOB_RECURSE FRONTEND_QML frontend/qml/*.qml )
file( GLOB_RECURSE FRONTEND_QMLDIR frontend/qml/qmldir )
file( GLOB_RECURSE FRONTEND_ASSETS
	frontend/qml/assets/*.jpg
	frontend/qml/assets/*.jpeg
	frontend/qml/assets/*.png
	frontend/qml/assets/*.svg
)

foreach( filepath_full ${FRONTEND_QML} ${FRONTEND_QMLDIR} ${FRONTEND_ASSETS} )
	# Strip source path to get relative path (in) then create final relative path (out)
	# Example: frontend/qml/Theme/PhxTheme.qml -> QML/Phoenix/Theme/PhxTheme.qml
	string( REPLACE ${CMAKE_SOURCE_DIR}/ "" filepath_relative_in ${filepath_full} )
	string( REPLACE frontend/qml/ "" filepath_relative_out ${filepath_relative_in} )

	# Capitalize first letter
	string( SUBSTRING ${filepath_relative_out} 0 1 first_letter )
	string( TOUPPER ${first_letter} first_letter )
	string( SUBSTRING ${filepath_relative_out} 1 -1 other_letters )

	# Put it back together
	string( CONCAT filepath_relative_out QML/Phoenix/ ${first_letter} ${other_letters} )

	# Debug
	#message( ${filepath_relative_in} " -> " ${filepath_relative_out} )

	configure_file( ${filepath_relative_in} ${filepath_relative_out} COPYONLY )
endforeach()

##
## qt.conf
##

# FIXME: Only use if you're gonna copy all the distributable stuff too
#configure_file( frontend/qt.conf qt.conf COPYONLY )

##
## SQLite
##

file( GLOB FRONTEND_METADATA frontend/metadata/*.sqlite )
set_source_files_properties( ${FRONTEND_METADATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/Metadata )

##
## Deployment
##

set( FRONTEND_CONTENT
	${FRONTEND_QML}
	${FRONTEND_QMLDIR}
	${FRONTEND_ASSETS}
	${FRONTEND_METADATA}
)

foreach( filepath ${FRONTEND_METADATA} )
	get_filename_component( filename ${filepath} NAME )
	configure_file( frontend/metadata/${filename} Metadata/${filename} COPYONLY )
endforeach()
