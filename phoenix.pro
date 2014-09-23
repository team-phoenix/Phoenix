
TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick sql concurrent

VERSION = 0.1

DEFINES += '"PHOENIX_VERSION=\\"$$VERSION\\""'

LIBS += -lSDL2

linux-g++ {
    QMAKE_CXXFLAGS_RELEASE = -D_FORTIFY_SOURCE=2

    # GCC >= 4.9
    system(g++ --version | grep -E -q -e '"4\.(9|[0-9]{2})"') {
        QMAKE_CXXFLAGS += -fstack-protector-strong
        QMAKE_CXXFLAGS_DEBUG += -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
        QMAKE_LFLAGS_DEBUG += -fsanitize=undefined -fsanitize=address
    }
}

win32 {
    QT +=  winextras gui-private
    LIBS += -LC:/SDL2/lib
    LIBS += -lmingw32 -lSDL2main -lSDL2
    LIBS += -lgdi32 -ldwmapi


    DEFINES += SDL_WIN
    INCLUDEPATH += C:/SDL2/include
}

HEADERS += include/core.h                      \
           include/videoitem.h                 \
           include/audio.h                     \
           include/audiobuffer.h               \
           include/sdlevents.h                 \
           include/joystick.h                  \
           include/joystickevents.h            \
           include/logging.h                   \
           include/qmlsettings.h               \
           include/inputmanager.h              \
           include/inputdevice.h               \
           include/inputdevicemapping.h        \
           include/inputdeviceevent.h          \
           include/keyboard.h                  \
           include/keyboardevents.h            \
           include/librarydbmanager.h          \
           include/gamelibrarymodel.h          \
           include/phoenixlibrary.h            \
           include/phoenixwindow.h             \
           include/inputdevicemappingfactory.h \
           include/inputdevicefactory.h        \
           include/thegamesdb.h                \
           include/cacheimage.h                \
           include/platformmanager.h

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
           src/phoenixlibrary.cpp              \
           src/phoenixwindow.cpp               \
           src/thegamesdb.cpp                  \
           src/cacheimage.cpp

RESOURCES = qml/qml.qrc assets/assets.qrc
