include( deployment.pri )

##
## Qt settings
##

    # Undefine this (for some reason it's on by default on Windows)
    CONFIG -= debug_and_release debug_and_release_target

    TEMPLATE += app

    QT += qml quick sql multimedia network

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
    INCLUDEPATH += ../backend ../backend/consumer ../backend/core ../backend/input ../backend/role ../backend/util
    INCLUDEPATH += cpp cpp/library cpp/library/database cpp/library/fileinfo cpp/library/model cpp/theme

    SOURCES += cpp/main.cpp \
               cpp/library/gamelauncher.cpp \
               cpp/library/imagecacher.cpp \
               cpp/library/phxpaths.cpp \
               cpp/library/database/libretrodatabase.cpp \
               cpp/library/database/metadatadatabase.cpp \
               cpp/library/database/userdatabase.cpp \
               cpp/library/fileinfo/archivefileinfo.cpp \
               cpp/library/fileinfo/biosfileinfo.cpp \
               cpp/library/fileinfo/cryptohash.cpp \
               cpp/library/fileinfo/cuefileinfo.cpp \
               cpp/library/fileinfo/gamefileinfo.cpp \
               cpp/library/model/collectionsmodel.cpp \
               cpp/library/model/coremodel.cpp \
               cpp/library/model/gamescanner.cpp \
               cpp/library/model/librarymodel.cpp \
               cpp/library/model/platformsmodel.cpp

    HEADERS += cpp/library/gamelauncher.h \
               cpp/library/imagecacher.h \
               cpp/library/phxpaths.h \
               cpp/library/database/libretrodatabase.h \
               cpp/library/database/metadatadatabase.h \
               cpp/library/database/userdatabase.h \
               cpp/library/fileinfo/archivefileinfo.h \
               cpp/library/fileinfo/biosfileinfo.h \
               cpp/library/fileinfo/cryptohash.h \
               cpp/library/fileinfo/cuefileinfo.h \
               cpp/library/fileinfo/gamefileinfo.h \
               cpp/library/model/collectionsmodel.h \
               cpp/library/model/coremodel.h \
               cpp/library/model/gamescanner.h \
               cpp/library/model/librarymodel.h \
               cpp/library/model/platformsmodel.h \
               cpp/theme/phoenixtheme.h

    PRECOMPILED_HEADER = cpp/frontendcommon.h

    RESOURCES += qml/qml.qrc \
                 qml/Theme/theme.qrc \
                 qml/assets/assets.qrc \
                 qml/BigPicture/bigpicture.qrc \
                 qml/MouseDrivenView/mousedrivenview.qrc \
                 qml/GameView/gameview.qrc

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
