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
    property alias coreControl: coreControl;
    property alias videoOutput: videoOutput;
    property bool showBar: false;
    property string title: "";
    property string artworkURL: "";

    // Object that handles the running game session
    CoreControl {
        id: coreControl;
        Component.onCompleted: {
            this.videoOutput = videoOutput;
            this.inputManager = root.inputManager;
        }

        vsync: true;
        volume: gameActionBar.volumeValue;

        // Use this to automatically play once loaded
        property bool autoPlay: false;
        property bool firstLaunch: true;

        onSourceChanged: {
            title = source[ "title" ];
            artworkURL = source[ "artworkURL" ];
            root.touchMode = source[ "core" ].indexOf( "desmume" ) > -1;
        }

        onStateChanged: {
            switch( state ) {
                case Control.STOPPED:
                    videoOutput.opacity = 0.0;
                    resetCursor();
                    cursorTimer.stop();
                    break;

                case Control.LOADING:
                    videoOutput.opacity = 0.0;
                    resetCursor();
                    cursorTimer.stop();
                    break;

                case Control.PLAYING:
                    root.title = title;

                    rootMouseArea.cursorShape = Qt.BlankCursor;

                    // Show the game content
                    videoOutput.opacity = 1.0;

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

                    videoOutput.opacity = 1.0;
                    resetCursor();
                    cursorTimer.stop();
                    break;

                case Control.UNLOADING:
                    root.title = "Unloading - " + title;
                    firstLaunch = true;
                    videoOutput.opacity = 0.0;
                    resetCursor();
                    cursorTimer.stop();
                    break;

                default:
                    break;
            }
        }
    }

    // A cool logo effect
    Item {
        id: gameViewBackground;
        anchors.fill: parent;
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
            enabled: videoOutput.opacity === 1.0 ? false : true;
        }

        // Glow effect for the logo
        // Only visible when a game is not running
        Glow {
            anchors.fill: phoenixLogo;
            source: phoenixLogo;
            color: "#d55b4a";
            radius: 8.0;
            samples: 16;
            enabled: videoOutput.opacity === 1.0 ? false : true;
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
            source: videoOutput;
            rotation: 180;
            radius: 64;
        }
    }

    // QML-based video output module
    VideoOutput {
        id: videoOutput;
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
        rotation: 180;

        linearFiltering: false;
        television: false;
        ntsc: true;
        widescreen: false;

        MouseArea {
            anchors.fill: parent;

            property real touchX: 0;
            property real touchY: 0;
            property point touch: Qt.point( 0, 0 );
            property bool touched: false;

            acceptedButtons: root.touchMode ? Qt.LeftButton | Qt.RightButton : Qt.LeftButton;
            onDoubleClicked: {
                if( root.touchMode ) {
                    mouse.accepted = false;
                } else {
                    if ( root.visibility === Window.FullScreen )
                        root.visibility = Window.Windowed;
                    else if ( root.visibility === Window.Windowed | Window.Maximized )
                        root.visibility = Window.FullScreen;
                }
            }
            onClicked: {
                if( root.touchMode ) {
                    if( mouse.button === Qt.RightButton ) {
                        // Toggle state
                        if( showBar ) showBar = false;
                        else showBar = true;
                    }
                }
            }
            onPressed: checkMouse( mouse );
            onReleased: forceAnEvent( mouse );
            onPositionChanged: checkMouse( mouse );

            function checkMouse( mouse ) {
                if( root.touchMode ) {
                    touch = Qt.point( ( width - mouseX ) / width, ( height - mouseY ) / height );
                    touched = pressedButtons & Qt.LeftButton;
                    // console.log( touch + " touched = " + touched + " (normal event)" );
                    root.inputManager.updateTouchState( touch, touched );
                }
            }

            // Send both messages to ensure it gets registered
            // Needed as the Apple trackpad sends pressed and released messages in quick succession
            // but not instantly. Some games cannot take one-frame input. InputManager uses a special latch
            // to ensure such one-frame inputs get played out over two frames.
            function forceAnEvent( mouse ) {
                if( root.touchMode && mouse.button === Qt.LeftButton ) {
                    // console.log( touch + " touched = true (event forced)" );
                    root.inputManager.updateTouchState( touch, true );
                    root.inputManager.updateTouchState( touch, false );
                }
            }
        }
    }

    // In-game controls
    GameActionBar {
        id: gameActionBar;
        anchors { bottom: parent.bottom; }
        anchors.horizontalCenter: parent.horizontalCenter;

        visible: opacity !== 0.0;
        enabled: visible;

        // gameActionBar visible only when paused or mouse recently moved and only while not transitioning
        opacity: {
            if( root.touchMode ) {
                opacity: 0.0;
                if( gameView.showBar || gameView.coreState === Control.PAUSED ) opacity: 1.0;
            } else {
                if( ( gameView.coreState === Control.PAUSED || cursorTimer.running || gameActionBarMouseArea.containsMouse )
                    && ( !layoutStackView.transitioning ) ) {
                    opacity: 1.0
                } else {
                    opacity: 0.0;
                }
            }
        }
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

    // Use this mouse area (which is the same size as GameActionBar) to see if the cursor is in that region
    Connections {
        target: gameActionBarMouseArea;
        onEntered: {
            cursorTimer.stop();
            resetCursor();
        }
        onExited: {
            mouseMoved();
        }
    }

    property Timer cursorTimer: Timer {
        interval: 1000;
        running: false;
        onTriggered: {
            // Don't hide anything in touch mode
            if( !root.touchMode ) hideCursor();
        }
    }

    // This function will reset the timer when called (which is whenever the mouse is moved)
    function mouseMoved() {
        // Reset the timer, show the mouse cursor and action bar (usually when mouse is moved)
        if( gameView.running && rootMouseArea.hoverEnabled ) {
            cursorTimer.restart();
            resetCursor();
        }
    }

    function hideCursor() { rootMouseArea.cursorShape = Qt.BlankCursor; }
    function resetCursor() { rootMouseArea.cursorShape = Qt.ArrowCursor; }
}
