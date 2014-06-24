TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include ../RetroArch
CONFIG += debug console

QT += widgets core gui multimedia qml quick

LIBS += 

# Input
HEADERS += include/Core.h include/openglwindow.h
SOURCES += src/main.cpp src/openglwindow.cpp src/Core.cpp

RESOURCES = qml.qrc
