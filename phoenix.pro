
TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick

VERSION = 0.1

DEFINES += '"PHOENIX_VERSION=\\"$$VERSION\\""'

LIBS += -lSDL2

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
           include/sdljoystick.h \
           include/logging.h     \
           include/inputmanager.h\
           include/inputdevice.h \
           include/keyboard.h

SOURCES += src/main.cpp          \
           src/videoitem.cpp     \
           src/core.cpp          \
           src/audio.cpp         \
           src/audiobuffer.cpp   \
           src/sdljoystick.cpp   \
           src/joystick.cpp      \
           src/logging.cpp       \
           src/inputmanager.cpp  \
           src/inputdevice.cpp   \
           src/keyboard.cpp

RESOURCES = qml/qml.qrc assets/assets.qrc
