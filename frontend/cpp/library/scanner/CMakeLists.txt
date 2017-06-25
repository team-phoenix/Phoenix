set(SOURCES_PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/gamehasher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gamehashercontroller.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mapfunctor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/reducefunctor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scannerutil.cpp
)

set(SOURCES_PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/gamehasher.h
    ${CMAKE_CURRENT_LIST_DIR}/gamehashercontroller.h
    ${CMAKE_CURRENT_LIST_DIR}/mapfunctor.h
    ${CMAKE_CURRENT_LIST_DIR}/reducefunctor.h
    ${CMAKE_CURRENT_LIST_DIR}/scannerutil.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
