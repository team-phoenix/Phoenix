list( APPEND SRC_LIST 
        backend/backendplugin.cpp
        backend/consumer/audiobuffer.cpp
        backend/consumer/audiooutput.cpp
        backend/consumer/videooutput.cpp
        backend/consumer/videooutputnode.cpp
        backend/control/controloutput.cpp
        backend/control/gameconsole.cpp
        backend/core/core.cpp
        backend/core/libretrocore.cpp
        backend/core/libretroloader.cpp
        backend/core/libretrorunner.cpp
        backend/core/libretrosymbols.cpp
        backend/core/libretrovariable.cpp
        backend/core/libretrovariableforwarder.cpp
        backend/core/libretrovariablemodel.cpp
        backend/input/gamepadstate.cpp
        backend/input/globalgamepad.cpp
        backend/input/keyboardstate.cpp
        backend/input/mousestate.cpp
        backend/input/remapper.cpp
        backend/input/remappermodel.cpp
        backend/input/sdlmanager.cpp
        backend/input/sdlunloader.cpp
        backend/pipeline/node.cpp
        backend/util/logging.cpp
        backend/util/microtimer.cpp
        backend/util/phoenixwindow.cpp
        backend/util/phoenixwindownode.cpp
)

# Only applies to OS X
if( APPLE )
    list( APPEND SRC_LIST 
        backend/util/osxhelper.mm
    )
endif()

# Embed the default controller database into the executable
set( BACKEND_QRC_LIST
    backend/input/controllerdb.qrc
)
