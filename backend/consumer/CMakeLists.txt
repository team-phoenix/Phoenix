set(SOURCES_PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/audiobuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audiooutput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/videooutput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/videooutputnode.cpp
)

set(SOURCES_PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/audiobuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/audiooutput.h
    ${CMAKE_CURRENT_LIST_DIR}/videooutput.h
    ${CMAKE_CURRENT_LIST_DIR}/videooutputnode.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
