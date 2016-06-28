target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/cuefile.cpp
	${CMAKE_CURRENT_LIST_DIR}/gamelauncher.cpp
	${CMAKE_CURRENT_LIST_DIR}/imagecacher.cpp
	${CMAKE_CURRENT_LIST_DIR}/librarytypes.cpp
	${CMAKE_CURRENT_LIST_DIR}/phxpaths.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/cuefile.h
	${CMAKE_CURRENT_LIST_DIR}/gamelauncher.h
	${CMAKE_CURRENT_LIST_DIR}/imagecacher.h
	${CMAKE_CURRENT_LIST_DIR}/librarytypes.h
	${CMAKE_CURRENT_LIST_DIR}/phxpaths.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
