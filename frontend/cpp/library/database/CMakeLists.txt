target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/databasehelper.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/databasehelper.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
