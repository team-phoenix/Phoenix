TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include ../RetroArch
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick

LIBS += 

HEADERS += include/core.h       \
           include/videoitem.h   \
           include/audio.h      \
           include/audiobuffer.h

SOURCES += src/main.cpp     \
           src/videoitem.cpp \
           src/core.cpp     \
           src/audio.cpp    \
           src/audiobuffer.cpp

RESOURCES = qml.qrc
