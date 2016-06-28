target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/controloutput.cpp
	${CMAKE_CURRENT_LIST_DIR}/gameconsole.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/controloutput.h
	${CMAKE_CURRENT_LIST_DIR}/gameconsole.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
