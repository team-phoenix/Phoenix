target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/coremodel.cpp
	${CMAKE_CURRENT_LIST_DIR}/databasesettings.cpp
	${CMAKE_CURRENT_LIST_DIR}/sqlcolumn.cpp
	${CMAKE_CURRENT_LIST_DIR}/sqlmodel.cpp
	${CMAKE_CURRENT_LIST_DIR}/sqlthreadedmodel.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/coremodel.h
	${CMAKE_CURRENT_LIST_DIR}/databasesettings.h
	${CMAKE_CURRENT_LIST_DIR}/sqlcolumn.h
	${CMAKE_CURRENT_LIST_DIR}/sqlmodel.h
	${CMAKE_CURRENT_LIST_DIR}/sqlthreadedmodel.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
