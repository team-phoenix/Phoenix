
TEMPLATE = app
TARGET = phoenix
INCLUDEPATH += ./include
CONFIG += debug console c++11

QT += widgets core gui multimedia qml quick sql concurrent

VERSION = 0.1

DEFINES += '"PHOENIX_VERSION=\\"$$VERSION\\""'

!macx {
    LIBS += -lSDL2
}
else {
    LIBS += -framework SDL2
}

linux-g++ {
    INCLUDEPATH += /usr/include/SDL2/
    QMAKE_CXXFLAGS_RELEASE = -D_FORTIFY_SOURCE=2
    LIBS += -lsoxr

    # GCC >= 4.9s
    system(g++ --version | grep -E -q -e '"4\.(9|[0-9]{2})"') {
        QMAKE_CXXFLAGS += -fstack-protector-strong
        QMAKE_CXXFLAGS_DEBUG += -fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer
        QMAKE_LFLAGS_DEBUG += -fsanitize=undefined -fsanitize=address
    }
}

macx {
    INCLUDEPATH += /Library/Frameworks/SDL2.framework/Headers
    QMAKE_CXXFLAGS += -F/Library/Frameworks
    QMAKE_LFLAGS += -F/Library/Frameworks
}

win32 {
    LIBS += -LC:/SDL2/lib
    LIBS += -lmingw32 -lSDL2main -lSDL2
    LIBS += -lgdi32 -ldwmapi


    DEFINES += SDL_WIN
    INCLUDEPATH += C:/SDL2/include

    #CONFIG(debug) {
    #    QMAKE_POST_LINK += $$QMAKE_COPY $${PWD}\\databases/systemdatabase.db $${OUT_PWD}\\debug $$escape_expand(\\n\\t)
    #    QMAKE_POST_LINK += $$QMAKE_COPY C:/SDL2/lib/SDL2.dll $${OUT_PWD}\\debug $$escape_expand(\\n\\t)
    #}

    #CONFIG(release) {
    #    QMAKE_POST_LINK += $$QMAKE_COPY $${PWD}\\databases/systemdatabase.db $${OUT_PWD}\\release $$escape_expand(\\n\\t)
    #    QMAKE_POST_LINK += $$QMAKE_COPY C:/SDL2/lib/SDL2.dll $${OUT_PWD}\\release $$escape_expand(\\n\\t)
    #}
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
           include/platformmanager.h           \
           include/coremodel.h                 \
           include/systemdatabase.h            \
           include/networkqueue.h              \
           include/scraper.h                   \

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
           src/cacheimage.cpp                  \
           src/coremodel.cpp                   \
           src/systemdatabase.cpp              \
           src/networkqueue.cpp                \
           src/scraper.cpp

RESOURCES = qml/qml.qrc assets/assets.qrc
