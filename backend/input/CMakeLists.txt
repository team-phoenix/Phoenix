set(SOURCES_PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/gamepadstate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/globalgamepad.cpp
    ${CMAKE_CURRENT_LIST_DIR}/keyboardstate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mousestate.cpp
    ${CMAKE_CURRENT_LIST_DIR}/remapper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/remappermodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sdlmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sdlunloader.cpp

    ${CMAKE_CURRENT_LIST_DIR}/controllerdb.qrc
)

set(SOURCES_PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/gamepadstate.h
    ${CMAKE_CURRENT_LIST_DIR}/globalgamepad.h
    ${CMAKE_CURRENT_LIST_DIR}/keyboardstate.h
    ${CMAKE_CURRENT_LIST_DIR}/mousestate.h
    ${CMAKE_CURRENT_LIST_DIR}/remapper.h
    ${CMAKE_CURRENT_LIST_DIR}/remappermodel.h
    ${CMAKE_CURRENT_LIST_DIR}/sdlmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/sdlunloader.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES_PRIVATE} PUBLIC ${SOURCES_PUBLIC})
source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES_PRIVATE} ${SOURCES_PUBLIC})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
