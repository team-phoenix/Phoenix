include( msvc.pri )

##
## Extra targets
##

    QMAKE_EXTRA_TARGETS += portable

##
## Qt settings
##

    # Undefine this for gcc (MINGW), it's not necessary
    gcc: CONFIG -= debug_and_release debug_and_release_target

    CONFIG += plugin qt static

    TEMPLATE = lib

    QT += qml quick multimedia

    # Needed to grab the native OpenGL context handle
    macx: QT += platformsupport-private

    TARGET = phoenix-backend

##
## Compiler settings
##

    CONFIG += c++11

    OBJECTS_DIR = obj
    MOC_DIR     = moc
    RCC_DIR     = rcc
    UI_DIR      = gui

    # Include libraries
    win32: msvc: INCLUDEPATH += $$SAMPLERATEBASE\include                                 # See msvc.pri
    win32: gcc:  INCLUDEPATH += C:/msys64/mingw64/include C:/msys64/mingw64/include/SDL2 # MSYS2
    win32: gcc:  INCLUDEPATH += /usr/lib/mxe/usr/x86_64-w64-mingw32.static/include/SDL2  # MXE (MinGW)
    macx:        INCLUDEPATH += /usr/local/include /usr/local/include/SDL2               # Homebrew
    macx:        INCLUDEPATH += /usr/local/include /opt/local/include/SDL2               # MacPorts
    unix:        INCLUDEPATH += /usr/include/SDL2                                        # Linux

    # Include externals
    DEFINES += QUAZIP_STATIC
    INCLUDEPATH += ../externals/quazip/quazip

    # Include our stuff
    INCLUDEPATH += consumer control core input pipeline util

    # Build with debugging info
    DEFINES += QT_MESSAGELOGCONTEXT

    HEADERS += \
    backendplugin.h \
    consumer/audiobuffer.h \
    consumer/audiooutput.h \
    consumer/videooutput.h \
    consumer/videooutputnode.h \
    control/controloutput.h \
    control/gameconsole.h \
    core/core.h \
    core/libretro.h \
    core/libretrocore.h \
    core/libretroloader.h \
    core/libretrorunner.h \
    core/libretrosymbols.h \
    core/libretrovariable.h \
    core/libretrovariablemodel.h \
    core/libretrovariableforwarder.h \
    input/gamepadstate.h \
    input/globalgamepad.h \
    input/keyboardstate.h \
    input/mousestate.h \
    input/remapper.h \
    input/remappermodel.h \
    input/sdlmanager.h \
    input/sdlunloader.h \
    pipeline/node.h \
    pipeline/pipelinecommon.h \
    util/logging.h \
    util/microtimer.h \
    util/phoenixwindow.h \
    util/phoenixwindownode.h \

    SOURCES += \
    backendplugin.cpp \
    consumer/audiobuffer.cpp \
    consumer/audiooutput.cpp \
    consumer/videooutput.cpp \
    consumer/videooutputnode.cpp \
    control/controloutput.cpp \
    control/gameconsole.cpp \
    core/core.cpp \
    core/libretrocore.cpp \
    core/libretroloader.cpp \
    core/libretrorunner.cpp \
    core/libretrosymbols.cpp \
    core/libretrovariable.cpp \
    core/libretrovariablemodel.cpp \
    core/libretrovariableforwarder.cpp \
    input/gamepadstate.cpp \
    input/globalgamepad.cpp \
    input/keyboardstate.cpp \
    input/mousestate.cpp \
    input/remapper.cpp \
    input/remappermodel.cpp \
    input/sdlmanager.cpp \
    input/sdlunloader.cpp \
    pipeline/node.cpp \
    util/logging.cpp \
    util/microtimer.cpp \
    util/phoenixwindow.cpp \
    util/phoenixwindownode.cpp \

    OBJECTIVE_SOURCES += \
    util/osxhelper.mm

    RESOURCES += input/controllerdb.qrc

##
## Linker settings
##

    ##
    ## Library paths
    ##

    # Externals
    LIBS += -L../externals/quazip/quazip

    # SDL2
    macx: LIBS += -L/usr/local/lib -L/opt/local/lib # Homebrew, MacPorts

    msvc: {
        LIBS += /LIBPATH:$$SAMPLERATEBASE\lib

        CONFIG( debug, debug|release ): {
            LIBS += /LIBPATH:../externals/quazip/quazip/debug
        }

        else {
            LIBS += /LIBPATH:../externals/quazip/quazip/release
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
