include( deployment.pri )
include( ../backend/msvc.pri )

##
## Qt settings
##

    # Undefine this for gcc (MINGW), it's not necessary
    gcc: CONFIG -= debug_and_release debug_and_release_target

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

    # Build with debugging info
    DEFINES += QT_MESSAGELOGCONTEXT

    # Version info
    win32: {
        # Grab current git tag (will be appended with revision hash if not on the commit the tag is tagged to)
        PHXVERSION = $$system( git describe --dirty )

        # Strip v from beginning of tag
        VERSION_TUPLE_1 = $$replace( PHXVERSION, v,  )

        # Strip everything after third number
        VERSION_TUPLE_3 = $$section( PHXVERSION, -, 0, 0 )

        # Put it all together
        VERSION_TUPLE = $$section( VERSION_TUPLE_1, ., 0, 0 ),$$section( PHXVERSION, ., 1, 1 ),$$section( VERSION_TUPLE_3, ., 2, 2 ),0
        DEFINES += PHOENIX_VER_STR=\"$$PHXVERSION\" PHOENIX_VER_TUPLE=$$VERSION_TUPLE
    }

    unix: {
        VERSION = $$system( git describe --dirty )
        DEFINES += PHOENIX_VER_STR=\"$$VERSION\"
    }

    # FIXME: Remove once newer Qt versions make this unnecessary
    macx: QMAKE_MAC_SDK = macosx10.11

    # SDL2/Qt main() conflict fix
    # Make sure that SDL.h is included *after* all other includes in main.cpp
    # http://blog.debao.me/2013/07/link-confilict-between-sdl-and-qt-under-windows/
    win32: QMAKE_LIBS_QT_ENTRY -= -lqtmain
    win32: gcc: DEFINES -= QT_NEEDS_QMAIN

    # Include libraries
    win32: msvc: INCLUDEPATH += $$SAMPLERATEBASE\include                                 # Manually built. Check msvc.pri
    win32: gcc:  INCLUDEPATH += C:/msys64/mingw64/include C:/msys64/mingw64/include/SDL2 # MSYS2
    macx:        INCLUDEPATH += /usr/local/include /usr/local/include/SDL2               # Homebrew
    macx:        INCLUDEPATH += /usr/local/include /opt/local/include/SDL2               # MacPorts
    unix:        INCLUDEPATH += /usr/include /usr/include/SDL2                           # Linux

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
    qml/qml.qrc \

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

    msvc: {
        LIBS += /LIBPATH:$$SAMPLERATEBASE\lib

        CONFIG( debug, debug|release ): {
            LIBS += /LIBPATH:../externals/quazip/quazip/debug
            LIBS += /LIBPATH:../backend/debug
        }

        else {
            LIBS += /LIBPATH:../externals/quazip/quazip/release
            LIBS += /LIBPATH:../backend/release
        }
    }

    ##
    ## Libraries
    ##

    # Externals
    LIBS += -lquazip

    !msvc {
        # SDL 2
        win32: LIBS += -lSDL2main
        LIBS += -lSDL2

        # Other libraries we use
        LIBS += -lsamplerate -lz
    }

    msvc: {
        LIBS += libsamplerate-0.lib
    }
