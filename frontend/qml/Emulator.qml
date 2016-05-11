import QtQuick 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

Item {
    // Make gameManager's objects available from emulator directly
    property alias controlOutput: gameConsole.controlOutput;
    property alias globalGamepad: gameConsole.globalGamepad;

    // Make Emulator's objects available unqualified
    property alias blurEffect: blurEffect;
    property alias gameConsole: gameConsole;
    property alias videoOutput: videoOutput;

    // A string that holds the title of the currently running game
    property string title: "";

    // A string that holds the URL for the front cover of the currently running game
    property string artworkURL: "";

    // Object that handles the running game session
    GameConsole {
        id: gameConsole;

        // Load a game if set on launch
        Component.onCompleted: {
            // Immediately launch a game if the command line was invoked
            // If this is set everything else should be, too
            if( commandLineSource[ "type" ] === "libretro" ) {
                // Set window title to game title
                title = commandLineSource[ "title" ];
                window.title = "Loading - " + title;

                // Set up the packet of information to pass to CoreControl
                var dict = {};
                dict[ "type" ] = "libretro";
                dict[ "core" ] = commandLineSource[ "core" ];
                dict[ "game" ] = commandLineSource[ "game" ];
                dict[ "systemPath" ] = PhxPaths.qmlFirmwareLocation();
                dict[ "savePath" ] = PhxPaths.qmlSaveLocation();

                // Extra stuff
                // title = commandLineSource[ "title" ];
                dict[ "title" ] = title;
                dict[ "system" ] = "";
                dict[ "artworkURL" ] = "";

                // Assign the source
                gameConsole.source = dict;

                // Connect the next callback in the chain to be called once the load begins/ends
                //controlOutput.stateChanged.connect( stateChangedCallback );

                // Begin the load
                // Execution will continue in stateChangedCallback() once CoreControl changes state
                gameConsole.load();
            }
        }

        aspectRatioMode: 0;
        vsync: true;
        volume: 1.0;
        userDataLocation: PhxPaths.qmlUserDataLocation();

        onSourceChanged: {
            title = source[ "title" ];
            artworkURL = source[ "artworkURL" ];
        }

        // An object that emits signals based on the state of all connected controllers combined
        // Use this to control the UI with any controller
        globalGamepad: GlobalGamepad {
            id: globalGamepad;
        }

        // A wrapper necessary to make PhoenixWindow part of the pipeline
        phoenixWindow: PhoenixWindowNode {
            id: phoenixWindowNode;
            phoenixWindow: window;
        }

        // A wrapper necessary to make VideoOutput part of the pipeline
        videoOutput: VideoOutputNode {
            id: videoOutputNode;
            videoOutput: videoOutput;
        }

        // Object used to track state changes and misc properties set from the core
        controlOutput: ControlOutput {
            id: controlOutput;

            // Use this to automatically play once loaded
            property bool autoPlay: true;
            property bool initialPauseNotYetDone: true;

            onStateChanged: {
                switch( state ) {
                    case Node.Stopped:
                        videoOutput.opacity = 0.0;
                        break;

                    case Node.Loading:
                        videoOutput.opacity = 0.0;
                        break;

                    case Node.Playing:
                        window.title = title;

                        // Show the game content
                        videoOutput.opacity = 1.0;

                        break;

                    case Node.Paused:
                        window.title = "Paused - " + title;
                        handleAutoPlay();
                        videoOutput.opacity = 1.0;
                        break;

                    case Node.Unloading:
                        window.title = "Unloading - " + title;
                        initialPauseNotYetDone = true;
                        videoOutput.opacity = 0.0;
                        break;

                    default:
                        break;
                }
            }

            function handleAutoPlay() {
                if( initialPauseNotYetDone ) {
                    initialPauseNotYetDone = false;
                    if( autoPlay ) {
                        console.log( "Autoplay activated" );
                        gameConsole.play();
                    }
                }
            }
        }
    }

    // A blurred copy of the video that sits behind the real video as an effect
    FastBlur {
        id: blurEffect;
        anchors.fill: parent;
        source: videoOutput;
        radius: 64;
    }

    // Custom video output module
    VideoOutput {
        id: videoOutput;
        anchors.centerIn: parent;

        // Scaling

        // Info for the various modes
        property real letterBoxHeight: parent.width / aspectRatio;
        property real letterBoxWidth: parent.width;
        property real pillBoxWidth: parent.height * aspectRatio;
        property real pillBoxHeight: parent.height;
        property bool pillBoxing: parent.width / parent.height / aspectRatio > 1.0;

        // Fit mode (0): Maintain aspect ratio, fit all content within window, letterboxing/pillboxing as necessary
        property real fitModeWidth: pillBoxing ? pillBoxWidth : letterBoxWidth;
        property real fitModeHeight: pillBoxing ? pillBoxHeight : letterBoxHeight;

        // Stretch mode (1): Fit to parent, ignore aspect ratio
        property real stretchModeWidth: parent.width;
        property real stretchModeHeight: parent.height;

        // Fill mode (2): Maintian aspect ratio, fill window with content, cropping the remaining stuff
        // TODO
        property real fillModeWidth: 0;
        property real fillModeHeight: 0;

        // Center mode (3): Show at core's native resolution
        // TODO
        property real centerModeWidth: 0;
        property real centerModeHeight: 0;

        onAspectModeChanged: calculateSize();
        Component.onCompleted: calculateSize();

        function calculateSize() {
            switch( aspectMode ) {
                case 0:
                    width = Qt.binding( function() { return fitModeWidth; } );
                    height = Qt.binding( function() { return fitModeHeight; } );
                    break;
                case 1:
                    width = Qt.binding( function() { return stretchModeWidth; } );
                    height = Qt.binding( function() { return stretchModeHeight; } );
                    break;
                case 2:
                    width = Qt.binding( function() { return fillModeWidth; } );
                    height = Qt.binding( function() { return fillModeHeight; } );
                    break;
                case 3:
                    width = Qt.binding( function() { return centerModeWidth; } );
                    height = Qt.binding( function() { return centerModeHeight; } );
                    break;
                default:
                    width = 0;
                    height = 0;
                    break;
            }
        }

        linearFiltering: false;
        television: false;
        ntsc: true;
        widescreen: false;
        }

    // A callback that begins playing a game once loading is complete
    // Disconnects itself from controlOutput once this is done
    function stateChangedCallback( newState ) {
        // Nothing to do, the load has begun
        if( newState === Node.Loading ) {
            console.log( "Core has begun loading." );
            return;
        }

        // Load complete, start game and hide library
        if( newState === Node.Paused ) {
            console.log( "Core loaded. Beginning emulation." );
            // Disconnect this callback once it's been used where we want it to be used
            controlOutput.stateChanged.disconnect( stateChangedCallback );

            gameConsole.play();

            return;
        }
    }
}
