target_sources( ${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/cmdlineargs.cpp
	${CMAKE_CURRENT_LIST_DIR}/logging.cpp
	${CMAKE_CURRENT_LIST_DIR}/main.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/cmdlineargs.h
	${CMAKE_CURRENT_LIST_DIR}/debughandler.h
	${CMAKE_CURRENT_LIST_DIR}/frontendcommon.h
	${CMAKE_CURRENT_LIST_DIR}/logging.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
