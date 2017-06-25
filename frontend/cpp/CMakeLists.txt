set(SOURCES_PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/cmdlineargs.cpp
    ${CMAKE_CURRENT_LIST_DIR}/logging.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

set(SOURCES_PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/cmdlineargs.h
    ${CMAKE_CURRENT_LIST_DIR}/debughandler.h
    ${CMAKE_CURRENT_LIST_DIR}/frontendcommon.h
    ${CMAKE_CURRENT_LIST_DIR}/logging.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
