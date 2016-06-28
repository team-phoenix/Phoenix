target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/archivefile.cpp
	${CMAKE_CURRENT_LIST_DIR}/cryptohash.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/archivefile.h
	${CMAKE_CURRENT_LIST_DIR}/cryptohash.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
