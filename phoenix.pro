TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include ../RetroArch
CONFIG += debug console

QT += widgets core gui multimedia qml quick

LIBS += 

HEADERS += include/core.h   \
           include/video-gl.h

SOURCES += src/main.cpp     \
           src/video-gl.cpp \
           src/core.cpp

RESOURCES = qml.qrc
