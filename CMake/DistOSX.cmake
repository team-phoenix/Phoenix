
set( OSX_ICON_FILES frontend/phoenix.icns )
set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

target_sources( ${PROJECT_NAME} PRIVATE ${OSX_ICON_FILES} ${FRONTEND_CONTENT} )

install( TARGETS Phoenix DESTINATION . )
