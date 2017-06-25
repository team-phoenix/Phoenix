set(
    SOURCES_PRIVATE
    #${CMAKE_CURRENT_LIST_DIR}/logging.cpp
    ${CMAKE_CURRENT_LIST_DIR}/microtimer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/phoenixwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/phoenixwindownode.cpp
)

set(SOURCES_PUBLIC
    #${CMAKE_CURRENT_LIST_DIR}/logging.h
    ${CMAKE_CURRENT_LIST_DIR}/microtimer.h
    ${CMAKE_CURRENT_LIST_DIR}/phoenixwindow.h
    ${CMAKE_CURRENT_LIST_DIR}/phoenixwindownode.h
)

if(APPLE)
        set(SOURCES_PRIVATE ${SOURCES_PRIVATE} ${CMAKE_CURRENT_LIST_DIR}/osxhelper.mm)
endif()

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
