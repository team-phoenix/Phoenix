set( OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/frontend/phoenix.icns )
set_source_files_properties( ${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
target_sources( ${PROJECT_NAME} PRIVATE ${OSX_ICON_FILES} )