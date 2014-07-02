TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include ../RetroArch
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick

LIBS += 

HEADERS += include/core.h       \
           include/video-gl.h   \
           include/audio.h      \
           include/audioio.h    \

SOURCES += src/main.cpp     \
           src/video-gl.cpp \
           src/core.cpp     \
           src/audio.cpp    \
           src/audioio.cpp  \

RESOURCES = qml.qrc
