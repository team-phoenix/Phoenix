import QtQuick 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

import "qrc:/Emulator"

MouseArea {
    // Make GameConsole's objects available from Emulator directly
    property alias controlOutput: gameConsole.controlOutput;
    property alias globalGamepad: gameConsole.globalGamepad;

    // Make Emulator's objects available unqualified
    property alias blurEffect: blurEffect;
    property alias gameConsole: gameConsole;
    property alias videoOutput: videoOutput;

    // A string that holds the title of the currently running game
    // Use window.title to read or write the window's title
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
                // Track the game's title
                title = commandLineSource[ "title" ];

                // Set up the packet of information to pass to GameConsole
                var dict = {};
                dict[ "type" ] = "libretro";
                dict[ "core" ] = commandLineSource[ "core" ];
                dict[ "game" ] = commandLineSource[ "game" ];
                dict[ "systemPath" ] = PhxPaths.qmlFirmwareLocation();
                dict[ "savePath" ] = PhxPaths.qmlSaveLocation();

                // Extra stuff
                dict[ "title" ] = title;
                dict[ "system" ] = "";
                dict[ "artworkURL" ] = "";

                // Assign the source
                gameConsole.source = dict;

                // Begin the load
                phoenix.state = "Loading";
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
        }
    }

    // A portion of this wrapper (the area the ActionBar overlaps) will be rendered into a texture so we can
    // apply a blur effect to it
    Item {
        id: videoOutputWrapper;
        anchors.fill: parent;

        // A blurred copy of the video that sits behind the real video as an effect
        FastBlur {
            id: blurEffect;
            anchors.fill: parent;
            source: videoOutput;
            radius: 64;
            opacity: desiredOpacity === 1.0 ? videoOutput.opacity : desiredOpacity;
            property real desiredOpacity: 1.0;
            Behavior on desiredOpacity { PropertyAnimation { duration: 250; } }
        }

        // Make sure something gets drawn when VideoOutput itself isn't being drawn
        Rectangle {
            anchors.fill: videoOutput;
            opacity: videoOutput.opacity;
            color: "black";
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

            property bool enableAnimation: false;
            Behavior on width { enabled: videoOutput.enableAnimation; PropertyAnimation { duration: 250; } }
            Behavior on height { enabled: videoOutput.enableAnimation; PropertyAnimation { duration: 250; } }

            linearFiltering: false;
            television: false;
            ntsc: true;
            widescreen: false;
        }
    }

    property string core: typeof gameConsole.source[ "core" ] === 'undefined' ? "" : gameConsole.source[ "core" ];

    // This will sample the area of videoItemWrapper that this ActionBar overlaps so we can apply a blur effect to it
    // This must be a sibling of videoItemWrapper to ensure we're using the right coordinate system
    ShaderEffectSource {
        id: videoOutputClip;
        width: 350;
        height: 45;
        visible: false;
        sourceItem: videoOutputWrapper;
        sourceRect: Qt.rect( actionBarCoords.x, actionBarCoords.y, actionBarMouseArea.width, actionBarMouseArea.height );
        property point actionBarCoords: mapToItem( videoOutputWrapper, actionBarMouseArea.x, actionBarMouseArea.y );
    }

    // A hint on how to hide the action bar (for touch mode)
    Text {
        id: actionBarHint;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: actionBarMouseArea.top;
        width: contentWidth * 2;
        height: contentHeight * 2;

        opacity: actionBarBlackRectangle.opacity;
        visible: core.indexOf( "desmume" ) > -1;

        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        text: "(Right-click to hide)";
        color: "white";
        style: Text.Outline;
        styleColor: "black";

        font {
            pixelSize: 12;
            family: PhxTheme.common.systemFontFamily;
        }
    }

    // Wrapper around ActionBar, lets us track when the user hovers over it
    MouseArea {
        id: actionBarMouseArea;

        hoverEnabled: true;

        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 10; }
        width: 350;
        height: 45;

        layer.enabled: true;

        Rectangle {
            id: actionBarBlackRectangle;
            anchors.fill: parent;
            color: "black";

            opacity: showCursor ? 1.0 : 0.0;
            enabled: showCursor;

            // Hide the ActionBar when the mouse doesn't cover it
            Behavior on opacity { PropertyAnimation { duration: 250; } }

            // A frosty glass background of the VideoOutput behind this
            FastBlur {
                anchors.fill: parent;
                source: videoOutputClip;
                radius: 64;
                opacity: 0.5;
            }

            // In-game controls
            ActionBar {
                id: actionBar;
                anchors.fill: parent;
            }
        }
    }

    // Right-click controllable toggle for touch mode
    acceptedButtons: Qt.LeftButton | Qt.RightButton;
    property bool touchModeShowActionBar: true;
    onClicked: if( ( mouse.button === Qt.RightButton ) && ( core.indexOf( "desmume" ) > -1 ) ) {
                   touchModeShowActionBar = touchModeShowActionBar ? false : true;
               }

    // Auto-hide the cursor
    hoverEnabled: true;
    property bool showCursor: core.indexOf( "desmume" ) > -1 ? ( touchModeShowActionBar )
                              : ( hideTimer.running || actionBarMouseArea.containsMouse || phoenix.state !== "Playing" );

    // Don't hide cursor if we're in touch mode
    onShowCursorChanged: {
        cursorShape = showCursor || ( core.indexOf( "desmume" ) > -1 ) ? Qt.ArrowCursor : Qt.BlankCursor;
    }

    // Reset the timer whenever the mouse moves
    onPositionChanged: hideTimer.restart();

    property Timer hideTimer: Timer {
        interval: 1500;
        running: true;
    }

    // Reset the timer on state change
    Connections {
        target: phoenix;
        onStateChanged: hideTimer.restart();
    }

    // Double-click to go fullscreen
    onDoubleClicked: if( ( mouse.button === Qt.LeftButton ) && !( core.indexOf( "desmume" ) > -1 ) )
                         window.toggleFullscreen();
}
