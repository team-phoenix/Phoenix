target_sources( ${PROJECT_NAME}
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/gstlibretroelement.c
	${CMAKE_CURRENT_LIST_DIR}/gstlibretroelement.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
