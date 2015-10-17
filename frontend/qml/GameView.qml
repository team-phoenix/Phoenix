import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

// Without deleting this component after every play session, we run the risk of a memory link from the core pointer not being cleared properly.
// This issue needs to be fixed.

Rectangle {
    id: gameView;
    color: PhxTheme.common.gameViewBackgroundColor;

    // Automatically set by VideoItem, true if a game is loaded and unpaused
    property bool running: videoItem.running;
    property alias coreState: videoItem.coreState;
    property alias loadedGame: videoItem.game;
    property alias videoItem: videoItem;

    // A small workaround to guarantee that the core and game are loaded in the correct order
    property var coreGamePair: {
        "corePath": ""
        , "gamePath": ""
        , "title": ""
    };

    // Changing this property triggers essentially launches the given game with the given core immediately
    onCoreGamePairChanged: {

        if ( coreGamePair[ "corePath" ] !== "" ) {
            console.log( "Attempting to load core: " + gameView.coreGamePair[ "corePath" ] );
            console.log( "Attempting to load game: " + gameView.coreGamePair[ "gamePath" ] );
        }

        videoItem.libretroCore = coreGamePair[ "corePath" ];
        videoItem.game = coreGamePair[ "gamePath" ];
    }

    // A logo
    // Only visible when a game is not running
    Image {
        id: phoenixLogo;
        anchors.centerIn: parent;
        width: 150;
        height: width;
        source: "phoenix.png";
        opacity: 0.25;
        enabled: videoItemContainer.opacity === 1.0 ? false : true;
    }

    // Glow effect for the logo
    // Only visible when a game is not running
    Glow {
        anchors.fill: phoenixLogo;
        source: phoenixLogo;
        color: "#d55b4a";
        radius: 8.0;
        samples: 16;
        enabled: videoItemContainer.opacity === 1.0 ? false : true;
        SequentialAnimation on radius {
            loops: Animation.Infinite;
            PropertyAnimation { to: 16; duration: 2000; easing.type: Easing.InOutQuart; }
            PropertyAnimation { to: 8; duration: 2000; easing.type: Easing.InOutQuart; }
        }
    }

    // A blurred copy of the video that sits behind the real video as an effect
    FastBlur {
        id: blurEffect;
        anchors.fill: parent;
        source: videoItemContainer;
        radius: 64;
    }

    // VideoItem serves simultaneously as a video output QML item (consumer) and as a "controller" for the
    // underlying emulation
    Rectangle {
        id: videoItemContainer;
        anchors { top: parent.top; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; }
        width: height * videoItem.aspectRatio;
        color: "black";
        opacity: 0;

        Behavior on opacity { NumberAnimation { duration: 250; } }

        VideoItem {
            id: videoItem;
            anchors.fill: parent;

            onCoreStateChanged: {
                switch( coreState ) {
                    case Core.STATEUNINITIALIZED:
                        resetCursor();
                        cursorTimer.stop();
                        break;
                    case Core.STATEREADY:
                        rootMouseArea.cursorShape = Qt.BlankCursor;

                        // Show the game content
                        videoItemContainer.opacity = 1.0;

                        // Let the window be resized even smaller than the default minimum size according to the aspect ratio
                        root.minimumWidth = Math.min( root.defaultMinWidth, root.defaultMinWidth / aspectRatio / 2);
                        root.minimumHeight = Math.min( root.defaultMinHeight, root.defaultMinHeight / aspectRatio / 2);

                        break;
                    case Core.STATEFINISHED:
                        resetCursor();
                        cursorTimer.stop();
                        break;
                    case Core.STATEERROR:
                        break;
                    case Core.STATEPAUSED:
                        resetCursor();
                        cursorTimer.stop();
                        break;
                    default:
                        break;
                }
            }

            rotation: 180;
            inputManager: root.inputManager;

            MouseArea {
                anchors.fill: parent;
                onDoubleClicked: {
                    if ( root.visibility === Window.FullScreen )
                        root.visibility = Window.Windowed;
                    else if ( root.visibility === Window.Windowed | Window.Maximized )
                        root.visibility = Window.FullScreen;
                }
            }
        }
    }

    GameActionBar {
        id: gameActionBar;
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
    }

    // Use the main mouse area to monitor the mouse for movement
    Connections {
        target: rootMouseArea;
        onPositionChanged: mouseMoved();
        onPressed: mouseMoved();
        onReleased: mouseMoved();
        onPressAndHold: mouseMoved();
    }

    property Timer cursorTimer: Timer {
        interval: 1000;
        running: false;
        onTriggered: { rootMouseArea.cursorShape = Qt.BlankCursor; }
    }

    // This function will reset the timer when called (which is whenever the mouse is moved)
    function mouseMoved() {
        // Reset the timer, show the mouse cursor and action bar (usually when mouse is moved)
        if( gameView.running && rootMouseArea.hoverEnabled ) {
            cursorTimer.restart();
            resetCursor();
        }
    }

    function resetCursor() { if( rootMouseArea.cursorShape !== Qt.ArrowCursor ) rootMouseArea.cursorShape = Qt.ArrowCursor; }
}
