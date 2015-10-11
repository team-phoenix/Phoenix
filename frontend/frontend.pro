# Targets

##
## Phoenix executable (default target)
##

TARGET = Phoenix

##
## Common
##

# Get the source and target's path
# On Windows, the DOS qmake paths must be converted to Unix paths as the GNU coreutils we'll be using expect that
# The default prefix is a folder called "Phoenix" at the root of the build folder
win32 {
    CONFIG( debug, debug|release ) {
        SOURCE_PATH = $$system( cygpath -u \"$$PWD\" )
        TARGET_PATH = $$system( cygpath -u \"$$OUT_PWD/debug\" )
    }

    CONFIG( release, debug|release ) {
        SOURCE_PATH = $$system( cygpath -u \"$$PWD\" )
        TARGET_PATH = $$system( cygpath -u \"$$OUT_PWD/release\" )
    }

    isEmpty( PREFIX ) { PREFIX = $$system( cygpath -u \"$$OUT_PWD\..\Phoenix-dist\" ) }
    else { PREFIX = $$system( cygpath -u \"$$PREFIX\" ) }
    PREFIX_WIN = $$system( cygpath -w \"$$PREFIX\" )
}

!win32 {
    SOURCE_PATH = $$PWD
    TARGET_PATH = $$OUT_PWD

    # On OS X, write directly to within the .app folder as that's where the executable lives
    macx: TARGET_APP = "$$sprintf( "%1/%2.app", $$OUT_PWD, $$TARGET )"
    macx: TARGET_PATH = "$$TARGET_APP/Contents/MacOS"
    isEmpty( PREFIX ) { PREFIX = $$OUT_PWD/../Phoenix-dist }
}

# Force the Phoenix binary to be relinked if the backend code has changed
win32: CONFIG(debug, debug|release): TARGETDEPS += ../backend/debug/libphoenix-backend.a
win32: CONFIG(release, debug|release): TARGETDEPS += ../backend/release/libphoenix-backend.a
!win32: TARGETDEPS += ../backend/libphoenix-backend.a

# Make sure it gets installed
target.path = "$$PREFIX"
unix: !macx: target.path = "$$PREFIX/bin"
INSTALLS += target

##
## Make sure that the portable file gets made in the build folder
##

PORTABLE_FILENAME = PHOENIX-PORTABLE

# For the default target (...and anything that depends on it)
QMAKE_POST_LINK += touch \"$$TARGET_PATH/$$PORTABLE_FILENAME\"

# Delete it from the prefix if doing a make install
portablefile.path = "$$PREFIX"
portablefile.extra = rm -f \"$$PREFIX/$$PORTABLE_FILENAME\"

# Make qmake aware that this target exists
QMAKE_EXTRA_TARGETS += portablefile

##
## Portable distribution: Copy just the files needed for a portable build to the given prefix so it can be archived
## and distributed
##

portable.depends = first portablefile

# Make qmake aware that this target exists
QMAKE_EXTRA_TARGETS += portable

# On OS X, just copy the whole .app folder to the prefix
macx {
    portable.commands += mkdir -p \"$$PREFIX/\" &\
                         cp -R -f \"$$TARGET_APP\" \"$$PREFIX/\"
}

# Everywhere else, copy the structure verbatim into the prefix
!macx {
    # Phoenix executable and the file that sets it to portable mode
    portable.commands += mkdir -p \"$$PREFIX/\" &\
                         cp -f \"$$TARGET_PATH/$$TARGET\" \"$$PREFIX/$$TARGET\" &\
                         cp -f \"$$TARGET_PATH/$$PORTABLE_FILENAME\" \"$$PREFIX/$$PORTABLE_FILENAME\" &\

    # Windows dependencies
    win32: portable.commands += cp \"/mingw64/bin/SDL2.dll\" \"$$TARGET_PATH/\" &\

    # Metadata databases
    portable.commands += mkdir -p \"$$PREFIX/metadata/\" &\
                         cp -f \"$$TARGET_PATH/metadata/openvgdb.sqlite\" \"$$PREFIX/metadata/openvgdb.sqlite\" &\
                         cp -f \"$$TARGET_PATH/metadata/systems.sqlite\" \"$$PREFIX/metadata/systems.sqlite\"
}

##
## Windows dependencies
##

win32 {
    windep.depends += "C:\msys64\mingw64\bin\SDL2.dll"

    # For the default target (...and anything that depends on it)
    windep.commands += cp \"/mingw64/bin/SDL2.dll\" \"$$TARGET_PATH/\"
    POST_TARGETDEPS += windep

    # For make install
    windep.files += "C:\msys64\mingw64\bin\SDL2.dll"
    windep.path = "$$PREFIX"
    INSTALLS += windep

    # Make qmake aware that this target exists
    QMAKE_EXTRA_TARGETS += windep
}

##
## Metadata database targets
##

# Ideally these files should come from the build folder, however, qmake will not generate rules for them if they don't
# already exist
metadb.depends += "$$PWD/metadata/openvgdb.sqlite" \
                  "$$PWD/metadata/systems.sqlite"

# For the default target (...and anything that depends on it)
metadb.commands += mkdir -p \"$$TARGET_PATH/metadata/\" &\
                   cp -f \"$$SOURCE_PATH/metadata/openvgdb.sqlite\" \"$$TARGET_PATH/metadata/openvgdb.sqlite\" &\
                   cp -f \"$$SOURCE_PATH/metadata/systems.sqlite\" \"$$TARGET_PATH/metadata/systems.sqlite\"
POST_TARGETDEPS += metadb

# For make install
metadb.files += "$$PWD/metadata/openvgdb.sqlite" \
                "$$PWD/metadata/systems.sqlite"
metadb.path = "$$PREFIX/metadata"
unix: metadb.path = "$$PREFIX/share/phoenix/metadata"
INSTALLS += metadb

# Make qmake aware that this target exists
QMAKE_EXTRA_TARGETS += metadb

##
## On OS X, ignore all of the above when it comes to make install and just copy the whole .app folder verbatim
##

macx {
    macxinstall.path = "$$PREFIX/"
    macxinstall.extra = cp -R \"$$TARGET_APP\" \"$$PREFIX/\" &\
                        rm -f \"$$TARGET_PATH/$$PORTABLE_FILENAME\"

    # Note the lack of +
    INSTALLS = macxinstall
}

##
## Debugging info
##

# win32 {
#     !build_pass: message( PWD: $$PWD )
#     !build_pass: message( OUT_PWD: $$OUT_PWD )
# }
# !build_pass: message( SOURCE_PATH: $$SOURCE_PATH )
# !build_pass: message( TARGET_PATH: $$TARGET_PATH )
# !build_pass: message( TARGET: $$TARGET )
# win32 {
#     !build_pass: message( PREFIX: $$PREFIX )
#     !build_pass: message( PREFIX_WIN: $$PREFIX_WIN )
# }

##
## Build settings
##

TEMPLATE += app

QT += qml quick widgets sql multimedia network

CONFIG += c++11 lib_bundle

# Include externals
INCLUDEPATH += ../externals/quazip/quazip

# Inlcude backend path
INCLUDEPATH += ../backend ../backend/input

LIBS += -L../externals/quazip/quazip -lquazip
LIBS += -L../backend -lphoenix-backend
LIBS += -lsamplerate -lz

# Windows-style
win32 {
    CONFIG -= windows
    QMAKE_LFLAGS += $$QMAKE_LFLAGS_WINDOWS

    # On Windows, the libraries live in debug and release folders
    LIBS += -L../externals/quazip/quazip/debug -L../externals/quazip/quazip/release
    LIBS += -L../backend/debug -L../backend/release

    LIBS += -lmingw32 -lSDL2main -lSDL2 -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid

    DEFINES += SDL_WIN
    INCLUDEPATH += C:/msys64/mingw64/include/SDL2 C:/msys64/mingw32/include/SDL2

    RC_FILE = ../phoenix.rc
}

macx {
    QMAKE_MAC_SDK = macosx10.11
    ICON = ../phoenix.icns
}

# Unix-style
unix {
    LIBS += -lSDL2
    INCLUDEPATH += /usr/local/include /usr/local/include/SDL2 # Homebrew (OS X)
    INCLUDEPATH += /opt/local/include /opt/local/include/SDL2 # MacPorts (OS X)
    INCLUDEPATH += /usr/include /usr/include/SDL2 # Linux
    QMAKE_CXXFLAGS +=
    QMAKE_LFLAGS += -L/usr/local/lib -L/opt/local/lib
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
           cpp/library/collectionsmodel.cpp \
           cpp/library/platform.cpp \
           cpp/library/systemdatabase.cpp \
           cpp/library/gamelauncher.cpp

HEADERS += cpp/library/librarymodel.h \
           cpp/library/libraryinternaldatabase.h \
           cpp/library/platforms.h \
           cpp/library/metadatadatabase.h \
           cpp/library/libraryworker.h \
           cpp/library/imagecacher.h \
           cpp/library/platformsmodel.h \
           cpp/library/phxpaths.h \
           cpp/library/collectionsmodel.h \
           cpp/library/platform.h \
           cpp/library/systemdatabase.h \
           cpp/library/gamelauncher.h

RESOURCES += qml/qml.qrc \
             qml/Theme/theme.qrc \
             qml/assets/assets.qrc \
             qml/BigPicture/bigpicture.qrc

DISTFILES += \
    qml/Theme/qmldir
