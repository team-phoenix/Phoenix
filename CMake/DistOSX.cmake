set( OSX_ICON_FILES frontend/phoenix.icns )
set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

target_sources( ${PROJECT_NAME} PRIVATE ${OSX_ICON_FILES} ${FRONTEND_CONTENT} )

install( TARGETS Phoenix DESTINATION . )

# This tool automatically patches hard-coded dependencies and copies them to the .app bundle
set(QT5_MACDEPLOYQT ${Qt5_DIR}/../../../bin/macdeployqt)
install(CODE "
	message(\"Executing macdeployqt...\")
	execute_process(COMMAND \"${QT5_MACDEPLOYQT}\" \"${CMAKE_INSTALL_PREFIX}/Phoenix.app\" -no-strip \"-qmldir=${CMAKE_INSTALL_PREFIX}/Phoenix.app/Contents/Resources/QML\")
	message(\"macdeployqt finished!\")
	")
