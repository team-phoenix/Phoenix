target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/gamehasher.cpp
	${CMAKE_CURRENT_LIST_DIR}/gamehashercontroller.cpp
	${CMAKE_CURRENT_LIST_DIR}/mapfunctor.cpp
	${CMAKE_CURRENT_LIST_DIR}/reducefunctor.cpp
	${CMAKE_CURRENT_LIST_DIR}/scannerutil.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/gamehasher.h
	${CMAKE_CURRENT_LIST_DIR}/gamehashercontroller.h
	${CMAKE_CURRENT_LIST_DIR}/mapfunctor.h
	${CMAKE_CURRENT_LIST_DIR}/reducefunctor.h
	${CMAKE_CURRENT_LIST_DIR}/scannerutil.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
