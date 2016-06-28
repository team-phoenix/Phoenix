target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/node.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/node.h
	${CMAKE_CURRENT_LIST_DIR}/pipelinecommon.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
