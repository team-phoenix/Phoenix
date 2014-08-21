
TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick sql

VERSION = 0.1

DEFINES += '"PHOENIX_VERSION=\\"$$VERSION\\""'

LIBS += -lSDL2

win32 {
    LIBS += -LC:/SDL2/lib
    LIBS += -lmingw32 -lSDL2main -lSDL2

    DEFINES += SDL_WIN
    INCLUDEPATH += C:/SDL2/include
}

HEADERS += include/core.h                      \
           include/videoitem.h                 \
           include/audio.h                     \
           include/audiobuffer.h               \
           include/sdlevents.h                 \
           include/joystick.h                  \
           include/logging.h                   \
           include/qmlsettings.h               \
           include/inputmanager.h              \
           include/inputdevice.h               \
           include/inputdevicemapping.h        \
           include/keyboard.h                  \
           include/librarydbmanager.h          \
           include/gamelibrarymodel.h          \
           include/phoenixwindow.h             \
           include/inputdevicemappingfactory.h \
           include/inputdevicefactory.h        \
           include/thegamesdb.h                \
           include/cacheimage.h

SOURCES += src/main.cpp                        \
           src/videoitem.cpp                   \
           src/core.cpp                        \
           src/audio.cpp                       \
           src/audiobuffer.cpp                 \
           src/sdlevents.cpp                   \
           src/joystick.cpp                    \
           src/logging.cpp                     \
           src/inputmanager.cpp                \
           src/inputdevice.cpp                 \
           src/inputdevicemapping.cpp          \
           src/keyboard.cpp                    \
           src/librarydbmanager.cpp            \
           src/gamelibrarymodel.cpp            \
           src/phoenixwindow.cpp               \
           src/thegamesdb.cpp                  \
           src/cacheimage.cpp

RESOURCES = qml/qml.qrc assets/assets.qrc
