include( deployment.pri )

##
## Qt settings
##

    # Undefine this (for some reason it's on by default on Windows)
    CONFIG -= debug_and_release debug_and_release_target

    TEMPLATE += app

    QT += qml quick sql multimedia network concurrent

##
## Compiler settings
##

    CONFIG += c++14

    OBJECTS_DIR = obj
    MOC_DIR     = moc
    RCC_DIR     = rcc
    UI_DIR      = gui

    # Version info
    win32: {
        # Grab current git tag (will be appended with revision hash if not on the commit the tag is tagged to)
        VERSION = $$system( C:\msys64\usr\bin\git describe )

        # Strip v from beginning of tag
        VERSION_TUPLE_1 = $$replace( VERSION, v,  )

        # Strip everything after third number
        VERSION_TUPLE_3 = $$section( VERSION, -, 0, 0 )

        # Put it all together
        VERSION_TUPLE = $$section( VERSION_TUPLE_1, ., 0, 0 ),$$section( VERSION, ., 1, 1 ),$$section( VERSION_TUPLE_3, ., 2, 2 ),0
        DEFINES += PHOENIX_VER_STR=\"$$VERSION\" PHOENIX_VER_TUPLE=$$VERSION_TUPLE
    }

    unix: {
        VERSION = $$system( git describe )
        DEFINES += PHOENIX_VER_STR=\"$$VERSION\"
    }

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
    INCLUDEPATH += ../backend ../backend/consumer ../backend/control ../backend/core ../backend/input ../backend/pipeline ../backend/util
    INCLUDEPATH += cpp cpp/library cpp/library/database cpp/library/fileinfo cpp/library/model cpp/library/scanner cpp/theme

    SOURCES += \
    cpp/library/cuefile.cpp \
    cpp/library/fileinfo/archivefile.cpp \
    cpp/library/fileinfo/cryptohash.cpp \
    cpp/library/gamelauncher.cpp \
    cpp/library/imagecacher.cpp \
    cpp/library/librarytypes.cpp \
    cpp/library/model/databasesettings.cpp \
    cpp/library/model/sqlcolumn.cpp \
    cpp/library/model/sqlmodel.cpp \
    cpp/library/model/sqlthreadedmodel.cpp \
    cpp/library/model/coremodel.cpp \
    cpp/library/phxpaths.cpp \
    cpp/library/scanner/gamehasher.cpp \
    cpp/library/scanner/gamehashercontroller.cpp \
    cpp/library/scanner/mapfunctor.cpp \
    cpp/library/scanner/reducefunctor.cpp \
    cpp/library/scanner/scannerutil.cpp \
    cpp/main.cpp \
    cpp/library/database/databasehelper.cpp \
    cpp/cmdlineargs.cpp \
    cpp/logging.cpp

    HEADERS += \
    cpp/library/cuefile.h \
    cpp/library/fileinfo/archivefile.h \
    cpp/library/fileinfo/cryptohash.h \
    cpp/library/gamelauncher.h \
    cpp/library/imagecacher.h \
    cpp/library/librarytypes.h \
    cpp/library/model/databasesettings.h \
    cpp/library/model/sqlcolumn.h \
    cpp/library/model/sqlmodel.h \
    cpp/library/model/sqlthreadedmodel.h \
    cpp/library/model/coremodel.h \
    cpp/library/phxpaths.h \
    cpp/library/scanner/gamehasher.h \
    cpp/library/scanner/gamehashercontroller.h \
    cpp/library/scanner/mapfunctor.h \
    cpp/library/scanner/reducefunctor.h \
    cpp/library/scanner/scannerutil.h \
    cpp/frontendcommon.h \
    cpp/library/database/databasehelper.h \
    cpp/cmdlineargs.h \
    cpp/debughelper.h \
    cpp/logging.h

    RESOURCES += \
    qml/qmlold.qrc \
    qml/Theme/theme.qrc \
    qml/MouseDrivenView/mousedrivenview.qrc \
    qml/GameView/gameview.qrc \
    qml/qml.qrc

##
## Linker settings
##

    ##
    ## General
    ##

    # Ensure that qDebug() messages and the like get written to stdout/stderr instead of the Windows debugger
    # This will set the exe's subsystem to "console" instead of "windows"
    win32: CONFIG( debug, debug|release ): CONFIG += console

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

    # SDL 2
    win32: LIBS += -lmingw32 -lSDL2main
    LIBS += -lSDL2

    # Other libraries we use
    LIBS += -lsamplerate -lz
