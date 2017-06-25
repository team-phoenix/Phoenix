target_sources( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/frontend/phoenix.rc )

install( TARGETS Phoenix RUNTIME DESTINATION . )
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QML DESTINATION . )
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Metadata DESTINATION . )

if( PORTABLE_MODE )
	install( FILES ${CMAKE_CURRENT_BINARY_DIR}/PHOENIX-PORTABLE DESTINATION . )
endif()

# This tool automatically gathers Qt dependencies and copies them to the install folder
set(QT5_WINDEPLOYQT ${Qt5_DIR}/../../../bin/windeployqt)
#get_filename_component(SDL2_DIR_PARENT "${SDL2_DIR}" DIRECTORY)
#--pdb
install(CODE "
        message(\"Executing windeployqt...\")
        execute_process(COMMAND \"${QT5_WINDEPLOYQT}\"
            --compiler-runtime
            --qmldir \"${CMAKE_INSTALL_PREFIX}/QML\"
            \"${CMAKE_INSTALL_PREFIX}/Phoenix.exe\"
        )
        message(\"windeployqt finished!\")
        ")
