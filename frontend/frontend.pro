TEMPLATE += app

QT += qml quick widgets sql multimedia network

CONFIG += c++11 lib_bundle

#Inlcude backend path
INCLUDEPATH += ../backend ../backend/input

LIBS += -L../backend -L../backend/debug -L../backend/release -lphoenix-backend
LIBS += -lsamplerate

win32 {
    CONFIG -= windows
    QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS

    LIBS += -LC:/SDL2/lib
    LIBS += -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid

    DEFINES += SDL_WIN
    INCLUDEPATH += C:/SDL2/include C:/msys64/mingw64/include/SDL2 C:/msys64/mingw32/include/SDL2

    CONFIG(debug, debug|release)  {
        depends.path = $$OUT_PWD
        depends.files += C:/msys64/mingw64/bin/SDL2.dll
        depends.files += $${PWD}/metadata/openvgdb.sqlite

    }

    CONFIG(release, debug|release) {
        depends.path = $$OUT_PWD
        depends.files += C:/msys64/mingw64/bin/SDL2.dll
        depends.files += $${PWD}/metadata/openvgdb.sqlite
    }

    INSTALLS += depends
    RC_FILE = ../phoenix.rc
}

else {
    LIBS += -lSDL2
    INCLUDEPATH += /usr/local/include /usr/local/include/SDL2 # Homebrew (OS X)
    INCLUDEPATH += /opt/local/include /opt/local/include/SDL2 # MacPorts (OS X)
    INCLUDEPATH += /usr/include /usr/include/SDL2 # Linux
    QMAKE_CXXFLAGS +=
    QMAKE_LFLAGS += -L/usr/local/lib -L/opt/local/lib
}

linux {

    CONFIG( debug, debug|release )  {
        depends.path = $$OUT_PWD/debug
        depends.files += $${PWD}/metadata/openvgdb.sqlite
    }


    CONFIG(release, debug|release) {
        depends.path = $$OUT_PWD/release
        depends.files += $${PWD}/metadata/openvgdb.sqlite
    }

    INSTALLS += depends

}

INCLUDEPATH += cpp/library

SOURCES += cpp/main.cpp \
           cpp/library/librarymodel.cpp \
           cpp/library/libraryinternaldatabase.cpp \
           cpp/library/platforms.cpp \
           cpp/library/metadatadatabase.cpp \
           cpp/library/libraryworker.cpp \
           cpp/library/imagecacher.cpp \
           cpp/library/platformsmodel.cpp \
           cpp/library/phxpaths.cpp \
    cpp/library/collectionsmodel.cpp

HEADERS += cpp/library/librarymodel.h \
           cpp/library/libraryinternaldatabase.h \
           cpp/library/libretro_cores_info_map.h \
           cpp/library/platforms.h \
           cpp/library/metadatadatabase.h \
           cpp/library/libraryworker.h \
           cpp/library/imagecacher.h \
           cpp/library/platformsmodel.h \
           cpp/library/phxpaths.h \
    cpp/library/collectionsmodel.h
# Will build the final executable in the main project directory.
TARGET = ../phoenix

# Check if the config file exists
include( ../common.pri )

RESOURCES += qml/qml.qrc \
             qml/Theme/theme.qrc \
             qml/assets/assets.qrc \
             qml/BigPicture/bigpicture.qrc

macx {
        depends.files += $${PWD}/metadata/openvgdb.sqlite
        depends.path = Contents/MacOS

        QMAKE_BUNDLE_DATA += depends
}

DISTFILES += \
    qml/Theme/qmldir
