target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/audiobuffer.cpp
	${CMAKE_CURRENT_LIST_DIR}/audiooutput.cpp
	${CMAKE_CURRENT_LIST_DIR}/videooutput.cpp
	${CMAKE_CURRENT_LIST_DIR}/videooutputnode.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/audiobuffer.h
	${CMAKE_CURRENT_LIST_DIR}/audiooutput.h
	${CMAKE_CURRENT_LIST_DIR}/videooutput.h
	${CMAKE_CURRENT_LIST_DIR}/videooutputnode.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
