import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.paths 1.0

// Without deleting this component after every play session, we run the risk of a memory link from the core pointer not being cleared properly.
// This issue needs to be fixed.

Rectangle {
    id: gameView;
    color: PhxTheme.common.gameViewBackgroundColor;

    // Automatically set by VideoItem, true if a game is loaded and unpaused
    property bool running: coreControl.state === Control.PLAYING;
    property alias coreState: coreControl.state;
    // property alias loadedGame: coreControl.source["game"];
    property alias coreControl: coreControl;
    property alias videoOutput: videoOutput;
    property string title: "";

    // A logo
    // Only visible when a game is not running
    Image {
        id: phoenixLogo;
        anchors.centerIn: parent;
        width: 150;
        height: width;
        source: "phoenix.png";
        sourceSize { height: height; width: width; }
        opacity: 0.25;
        enabled: coreControlContainer.opacity === 1.0 ? false : true;
    }

    // Glow effect for the logo
    // Only visible when a game is not running
    Glow {
        anchors.fill: phoenixLogo;
        source: phoenixLogo;
        color: "#d55b4a";
        radius: 8.0;
        samples: 16;
        enabled: coreControlContainer.opacity === 1.0 ? false : true;
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
        source: coreControlContainer;
        radius: 64;
    }

    Rectangle {
        id: coreControlContainer;
        anchors.centerIn: parent;

        // TODO: This sets up a "fit" strategy. If the user wants we should also have a
        // "stretch" (width/height = parent.width/height), "fill" (use aspectWidth/Height)
        // and "center" (have VideoOutput export nativeHeight/Width and clamp if larger than this container) strategy
        property int aspectWidth: parent.height * videoOutput.aspectRatio;
        property int aspectHeight: videoOutput.aspectRatio < 1.0 ? parent.height : parent.width / videoOutput.aspectRatio;

        property int fitModeWidth: aspectWidth > parent.width ? parent.width : aspectWidth;
        property int fitModeHeight: aspectHeight > parent.height ? parent.height : aspectHeight;

        width: fitModeWidth;
        height: fitModeHeight;

        color: "black";
        opacity: 0.0;

        Behavior on opacity { NumberAnimation { duration: 250; } }

        CoreControl {
            id: coreControl;
            Component.onCompleted: {
                this.videoOutput = videoOutput;
                this.inputManager = root.inputManager;
            }

            vsync: true;

            // Use this to automatically play once loaded
            property bool autoPlay: false;
            property bool firstLaunch: true;

            onSourceChanged: {
                title = source[ "title" ];
            }

            onStateChanged: {
                switch( state ) {
                    case Control.STOPPED:
                        coreControlContainer.opacity = 0.0;
                        resetCursor();
                        cursorTimer.stop();
                        break;

                    case Control.LOADING:
                        coreControlContainer.opacity = 0.0;
                        resetCursor();
                        cursorTimer.stop();
                        break;

                    case Control.PLAYING:
                        root.title = title;

                        rootMouseArea.cursorShape = Qt.BlankCursor;

                        // Show the game content
                        coreControlContainer.opacity = 1.0;

                        // Let the window be resized even smaller than the default minimum size according to the aspect ratio
                        root.minimumWidth = Math.min( root.defaultMinWidth, root.defaultMinWidth / videoOutput.aspectRatio / 2 );
                        root.minimumHeight = Math.min( root.defaultMinHeight, root.defaultMinHeight / videoOutput.aspectRatio / 2 );

                        break;

                    case Control.PAUSED:
                        root.title = "Paused - " + title;
                        if( firstLaunch ) {
                            firstLaunch = false;
                            if( autoPlay ) {
                                console.log( "Autoplay activated" );
                                play();
                            }
                        }

                        coreControlContainer.opacity = 1.0;
                        resetCursor();
                        cursorTimer.stop();
                        break;

                    case Control.UNLOADING:
                        root.title = "Unloading - " + title;
                        firstLaunch = true;
                        coreControlContainer.opacity = 0.0;
                        resetCursor();
                        cursorTimer.stop();
                        break;

                    default:
                        break;
                }
            }
        }

        VideoOutput {
            id: videoOutput;
            anchors.fill: parent;
            rotation: 180;
            MouseArea {
                anchors.fill: parent;
                onDoubleClicked: {
                    if ( root.visibility === Window.FullScreen )
                        root.visibility = Window.Windowed;
                    else if ( root.visibility === Window.Windowed | Window.Maximized )
                        root.visibility = Window.FullScreen;
                }
            }
            linearFiltering: false;
            television: false;
            ntsc: true;
            widescreen: false;
        }

    }

    GameActionBar {
        id: gameActionBar;
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
    }

    // Mouse stuff

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
