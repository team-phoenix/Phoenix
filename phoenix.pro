TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include ../RetroArch
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick

win32 {
    LIBS += -LC:/SDL2/lib
    LIBS += -lmingw32 -lSDL2main -lSDL2

    DEFINES += SDL_WIN
    INCLUDEPATH += C:/SDL2/include
}

HEADERS += include/core.h        \
           include/videoitem.h   \
           include/audio.h       \
           include/audiobuffer.h \
           include/joystick.h    \
           include/sdljoystick.h

SOURCES += src/main.cpp          \
           src/videoitem.cpp     \
           src/core.cpp          \
           src/audio.cpp         \
           src/audiobuffer.cpp   \
           src/sdljoystick.cpp   \
           src/joystick.cpp

RESOURCES = qml.qrc
