import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

// Without deleting this component after every play session, we run the risk of a memory link from the core pointer not being cleared properly.
// This issue needs to be fixed.

Rectangle {
    id: gameView;
    color: "black";

    // Automatically set by VideoItem, true if a game is loaded and unpaused
    property bool running: videoItem.running;
    property alias coreState: videoItem.coreState;
    property alias loadedGame: videoItem.game;
    property alias videoRender: videoItem;

    // A small workaround to guarantee that the core and game are loaded in the correct order
    // Use a dictionary for this, so we don't have to be restricted to a specific array order.
    property var coreGamePair: {
        "corePath": ""
        , "gamePath": ""
        , "title": ""
    };

    onCoreGamePairChanged: {
        console.log("corePath: " + gameView.coreGamePair[ "corePath"]  + " " + gameView.coreGamePair[ "gamePath" ] );

        //for ( var i in coreGamePair) {
          //  print( i + " value: " + coreGamePair[i])
            // }

        console.log( !videoItem.running)

        if( coreGamePair[ "corePath" ] !== "" && videoItem.coreState === Core.STATEUNINITIALIZED ) {
            videoItem.libretroCore = coreGamePair[ "corePath" ];
            videoItem.game = coreGamePair[ "gamePath" ];
        }
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

    /*
    // A toggle for the above blur effect... just in case this murders performance
    MouseArea {
        anchors.fill: parent;
        onDoubleClicked: {
            if( blurEffect.visible ) blurEffect.visible = false;
            else if( !blurEffect.visible ) blurEffect.visible = true;
        }
    }
    */

    // VideoItem serves simultaneously as a video output QML item (consumer) and as a "controller" for the
    // underlying emulation
    Rectangle {
        id: videoItemContainer;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            horizontalCenter: parent.horizontalCenter;
        }

        width: height * videoItem.aspectRatio;

        color: "black";
        opacity: 0;

        Behavior on opacity {
            NumberAnimation { duration: 250; }
        }

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

    DropShadow {
        anchors.fill: source;
        source: actionBar;
        horizontalOffset: 0;
        verticalOffset: 0;
        radius: 8.0;
        samples: radius * 2;
        color: "black";
        transparentBorder: true;
        opacity: actionBar.opacity;
    }

    Rectangle {
        id: actionBar;
        height: 50;

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#5f6273"; }
            GradientStop { position: 1.0; color: "#3f3c47"; }
        }

        // actionBar visible only when paused or mouse recently moved and only while not transitioning
        opacity: ( ( ( gameView.coreState === Core.STATEPAUSED ) || ( cursorTimer.running ) )
                    && ( !layoutStackView.transitioning ) ) ? 1.0 : 0.0;

        Behavior on opacity {
            PropertyAnimation { duration: 250; }
        }

        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
            bottomMargin: 50;
            leftMargin: 100;
            rightMargin: 100;
        }

        Row {
            id: mediaButtonsRow;
            anchors {
                fill: parent;
            }

            spacing: 0;

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: height;
                gradient: Gradient {
                    GradientStop { position: 0.0; color: videoItem.running ? "#d66b59" : "#8fc661"; }
                    GradientStop { position: 1.0; color: videoItem.running ? "#9a2954" : "#369657"; }
                }

                Rectangle {
                    id: topBorder;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 1;
                    color: "white";
                    opacity: 0.5;
                }

                Rectangle {
                    id: bottomBorder;
                    anchors {
                        bottom: parent.bottom;
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 1;
                    color: "white";
                    opacity: 0.05;

                }

                Rectangle {
                    id: leftBorder;
                    anchors {
                        top: topBorder.bottom;
                        left: parent.left;
                        bottom: bottomBorder.top;
                    }
                    width: 1;
                    color: "white";
                    opacity: 0.1;
                }

                Rectangle {
                    id: rightBorder;
                    anchors {
                        top: topBorder.bottom;
                        right: parent.right;
                        bottom: bottomBorder.top;
                    }
                    width: 1;
                    color: "white";
                    opacity: 0.1;
                }

                Label {
                    anchors {
                        centerIn: parent;
                    }
                    text: videoItem.running ? qsTr( "Pause" ) : qsTr( "Play" );
                    color: "white";
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if ( videoItem.running ) {
                            videoItem.pause();
                        } else {
                            videoItem.resume();
                        }
                    }
                }
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }

                width: 1;
                color: "black";
                opacity: 0.2;
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }

                color: "red";
                width: height;

                Label {
                    anchors.centerIn: parent;
                    color: "white";
                    text: qsTr( "Blur" );
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if( blurEffect.visible ) {
                            blurEffect.visible = false;
                        }
                        else if( !blurEffect.visible ) {
                            blurEffect.visible = true;
                        }
                    }
                }
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }

                color: "yellow";
                width: height;

                Label {
                    anchors.centerIn: parent;
                    text: qsTr( "ShutDown" );
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        videoItem.stop();
                        root.disableMouseClicks();
                        rootMouseArea.hoverEnabled = false;
                        resetCursor();
                        layoutStackView.push( mouseDrivenView );
                    }
                }
            }
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                right: parent.right;
            }

            color: "blue";
            width: height;

            Label {
                anchors.centerIn: parent;
                color: "white";
                text: qsTr( "Suspend" );
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    videoItem.pause();
                    root.disableMouseClicks();
                    rootMouseArea.hoverEnabled = false;
                    resetCursor();
                    layoutStackView.push( mouseDrivenView );
                }
            }
        }
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

        onTriggered: {
            rootMouseArea.cursorShape = Qt.BlankCursor;
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

    function resetCursor() {
        if( rootMouseArea.cursorShape !== Qt.ArrowCursor )
            rootMouseArea.cursorShape = Qt.ArrowCursor;
    }

}
