target_sources( ${PROJECT_NAME}
	PRIVATE
	#${CMAKE_CURRENT_LIST_DIR}/logging.cpp
	${CMAKE_CURRENT_LIST_DIR}/microtimer.cpp
	${CMAKE_CURRENT_LIST_DIR}/phoenixwindow.cpp
	${CMAKE_CURRENT_LIST_DIR}/phoenixwindownode.cpp

	PUBLIC
	#${CMAKE_CURRENT_LIST_DIR}/logging.h
	${CMAKE_CURRENT_LIST_DIR}/microtimer.h
	${CMAKE_CURRENT_LIST_DIR}/phoenixwindow.h
	${CMAKE_CURRENT_LIST_DIR}/phoenixwindownode.h
)

if( APPLE )
	target_sources( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/osxhelper.mm )
endif()

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
