include( deployment.pri )

##
## Qt settings
##

    # Undefine this (for some reason it's on by default on Windows)
    CONFIG -= debug_and_release debug_and_release_target

    TEMPLATE += app

    QT += qml quick widgets sql multimedia network

##
## Compiler settings
##

    CONFIG += c++11

    OBJECTS_DIR = obj
    MOC_DIR     = moc
    RCC_DIR     = rcc
    UI_DIR      = gui

    # FIXME: Remove once newer Qt versions make this unnecessary
    macx: QMAKE_MAC_SDK = macosx10.11

    # SDL 2
    # http://web.archive.org/web/20150305002626/http://blog.debao.me/2013/07/link-confilict-between-sdl-and-qt-under-windows/
    # Applies to both compiler and linker stages
    win32: CONFIG -= windows
    win32: QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS

    # Include libraries
    win32: INCLUDEPATH += C:/msys64/mingw64/include C:/msys64/mingw64/include/SDL2 # MSYS2
    macx:  INCLUDEPATH += /usr/local/include /usr/local/include/SDL2               # Homebrew
    macx:  INCLUDEPATH += /usr/local/include /opt/local/include/SDL2               # MacPorts
    unix:  INCLUDEPATH += /usr/include /usr/include/SDL2                           # Linux

    # Include externals
    DEFINES += QUAZIP_STATIC
    INCLUDEPATH += ../externals/quazip/quazip

    # Include our stuff
    INCLUDEPATH += ../backend ../backend/input
    INCLUDEPATH += cpp/library cpp

    SOURCES += cpp/main.cpp \
               cpp/library/librarymodel.cpp \
               cpp/library/libraryinternaldatabase.cpp \
               cpp/library/metadatadatabase.cpp \
               cpp/library/libraryworker.cpp \
               cpp/library/imagecacher.cpp \
               cpp/library/platformsmodel.cpp \
               cpp/library/phxpaths.cpp \
               cpp/library/collectionsmodel.cpp \
               cpp/library/systemdatabase.cpp \
               cpp/library/gamelauncher.cpp \
               cpp/library/gamefileinfo.cpp \
               cpp/library/cuefileinfo.cpp \
               cpp/library/biosfileinfo.cpp \
               cpp/library/archivefileinfo.cpp \
    cpp/library/cryptohash.cpp

    HEADERS += cpp/library/librarymodel.h \
               cpp/library/libraryinternaldatabase.h \
               cpp/library/metadatadatabase.h \
               cpp/library/libraryworker.h \
               cpp/library/imagecacher.h \
               cpp/library/platformsmodel.h \
               cpp/library/phxpaths.h \
               cpp/library/collectionsmodel.h \
               cpp/library/systemdatabase.h \
               cpp/library/gamelauncher.h \
               cpp/library/gamefileinfo.h \
               cpp/library/cuefileinfo.h \
               cpp/library/biosfileinfo.h \
               cpp/library/archivefileinfo.h \
    cpp/library/cryptohash.h

    PRECOMPILED_HEADER = cpp/frontendcommon.h

    RESOURCES += qml/qml.qrc \
                 qml/Theme/theme.qrc \
                 qml/assets/assets.qrc \
                 qml/BigPicture/bigpicture.qrc

##
## Linker settings
##

    ##
    ## Library paths
    ##

    # Externals
    LIBS += -L../externals/quazip/quazip

    # Our stuff
    LIBS += -L../backend

    # SDL2
    macx: LIBS += -L/usr/local/lib -L/opt/local/lib # Homebrew, MacPorts

    ##
    ## Libraries
    ##

    # Externals
    LIBS += -lquazip

    # Our stuff
    LIBS += -lphoenix-backend

    # SDL 2
    win32: LIBS += -lmingw32 -lSDL2main
    LIBS += -lSDL2

    # Other libraries we use
    LIBS += -lsamplerate -lz
