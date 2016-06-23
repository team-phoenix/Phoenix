list( APPEND SRC_LIST
    frontend/cpp/cmdlineargs.cpp
    frontend/cpp/library/cuefile.cpp
    frontend/cpp/library/database/databasehelper.cpp
    frontend/cpp/library/fileinfo/archivefile.cpp
    frontend/cpp/library/fileinfo/cryptohash.cpp
    frontend/cpp/library/gamelauncher.cpp
    frontend/cpp/library/imagecacher.cpp
    frontend/cpp/library/librarytypes.cpp
    frontend/cpp/library/model/coremodel.cpp
    frontend/cpp/library/model/databasesettings.cpp
    frontend/cpp/library/model/sqlcolumn.cpp
    frontend/cpp/library/model/sqlmodel.cpp
    frontend/cpp/library/model/sqlthreadedmodel.cpp
    frontend/cpp/library/phxpaths.cpp
    frontend/cpp/library/scanner/gamehasher.cpp
    frontend/cpp/library/scanner/gamehashercontroller.cpp
    frontend/cpp/library/scanner/mapfunctor.cpp
    frontend/cpp/library/scanner/reducefunctor.cpp
    frontend/cpp/library/scanner/scannerutil.cpp
    frontend/cpp/logging.cpp
    frontend/cpp/main.cpp
)

# QML
file( GLOB_RECURSE FRONTEND_QML frontend/qml/*.qml )
file( GLOB_RECURSE FRONTEND_ASSETS 
    frontend/qml/assets/*.jpg  
    frontend/qml/assets/*.jpeg  
    frontend/qml/assets/*.png 
    frontend/qml/assets/*.svg 
)

foreach( filepath_full ${FRONTEND_QML} ${FRONTEND_ASSETS} )
    # Strip source path to get relative path (in) then create final relative path (out)
    # Example: frontend/qml/Theme/PhxTheme.qml -> QML/Phoenix/Theme/PhxTheme.qml
    string( REPLACE ${CMAKE_SOURCE_DIR}/ "" filepath_relative_in ${filepath_full} )
    string( REPLACE frontend/qml QML/Phoenix filepath_relative_out ${filepath_relative_in} )

    # Extras
    string( REPLACE assets Assets filepath_relative_out ${filepath_relative_out} )
    #message( ${filepath_relative_in} " -> " ${filepath_relative_out} )

    configure_file( ${filepath_relative_in} ${filepath_relative_out} COPYONLY )
endforeach()

# SQLite
file( GLOB FRONTEND_METADATA frontend/metadata/*.sqlite )
set_source_files_properties( ${FRONTEND_METADATA} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/Metadata )

set( FRONTEND_CONTENT
    ${FRONTEND_QML} ${FRONTEND_ASSETS}
    ${FRONTEND_METADATA}
)

foreach( file ${FRONTEND_METADATA} )
    get_filename_component( filename ${file} NAME )
    configure_file( frontend/metadata/${filename} Metadata/${filename} COPYONLY )
endforeach()
