target_sources( ${PROJECT_NAME} 
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/core.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrocore.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretroloader.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrorunner.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrosymbols.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrovariable.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrovariableforwarder.cpp
	${CMAKE_CURRENT_LIST_DIR}/libretrovariablemodel.cpp

	PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/core.h
	${CMAKE_CURRENT_LIST_DIR}/libretro.h
	${CMAKE_CURRENT_LIST_DIR}/libretrocore.h
	${CMAKE_CURRENT_LIST_DIR}/libretroloader.h
	${CMAKE_CURRENT_LIST_DIR}/libretrorunner.h
	${CMAKE_CURRENT_LIST_DIR}/libretrosymbols.h
	${CMAKE_CURRENT_LIST_DIR}/libretrovariable.h
	${CMAKE_CURRENT_LIST_DIR}/libretrovariableforwarder.h
	${CMAKE_CURRENT_LIST_DIR}/libretrovariablemodel.h
)

target_include_directories( ${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}" )
