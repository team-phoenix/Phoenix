import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

Rectangle {
    id: gameView;
    color: "black";

    // Automatically set by VideoItem, true if a game is loaded and unpaused
    property bool running: false;

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

    // A small workaround to guarantee that the core and game are loaded in the correct order
    property var coreGamePair: [ "", "" ];
    onCoreGamePairChanged: {
        if( coreGamePair[0] !== "" ) {
            videoItem.libretroCore = coreGamePair[ 0 ];
            videoItem.game = coreGamePair[ 1 ];
        }
    }

    // A blurred copy of the video that sits behind the real video as an effect
    FastBlur {
        id: blurEffect;
        anchors.fill: parent;
        source: videoItemContainer;
        radius: 64;
    }

    // A toggle for the above blur effect... just in case this murders performance
    MouseArea {
        anchors.fill: parent;
        onDoubleClicked: {
            if( blurEffect.visible ) blurEffect.visible = false;
            else if( !blurEffect.visible ) blurEffect.visible = true;
        }
    }

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

            onAspectRatioChanged: {
                console.log( "Aspect ratio: " + aspectRatio );
            }

            onSignalRunChanged: {
                gameView.running = running;
                if( run === true ) {
                    rootMouseArea.cursorShape = Qt.ArrowCursor;
                    resetTimer();
                    videoItemContainer.opacity = 1.0;
                } else {
                    cursorTimer.stop();
                    if( rootMouseArea.cursorShape !== Qt.ArrowCursor )
                        rootMouseArea.cursorShape = Qt.ArrowCursor;
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

    Component {
        id: actionBarShadow;

        DropShadow {
            anchors.fill: source;
            source: actionBar;
            horizontalOffset: 0;
            verticalOffset: 0;
            radius: 8.0;
            samples: radius * 2;
            color: "black";
            transparentBorder: true;
        }
    }

    Rectangle {
        id: actionBar;

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#5f6273"; }
            GradientStop { position: 1.0; color: "#3f3c47"; }
        }

        opacity: videoItemContainer.opacity;
        height: 50;

        property var dropShadow: null;

        onOpacityChanged: {
            if ( opacity === 1.0 ) {
                actionBar.dropShadow = actionBarShadow.createObject( gameView );
            } else if ( opacity === 0.0 ) {
                if ( actionBar.dropShadow ) {
                    actionBar.dropShadow.destroy();
                }
            }
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
        }
    }

    // Use the main mouse area to monitor the mouse for movement
    Connections {
        target: rootMouseArea;
        onPositionChanged: resetTimer();
        onPressed: resetTimer();
        onReleased: resetTimer();
        onPressAndHold: resetTimer();
    }

    property Timer cursorTimer: Timer {
        interval: 1000;
        running: false;

        onTriggered: {
            rootMouseArea.cursorShape = Qt.BlankCursor;
        }
    }

    // This function will reset the timer when called (which is whenever the mouse is moved)
    function resetTimer() {
        if( rootMouseArea.cursorShape !== Qt.ArrowCursor )
            rootMouseArea.cursorShape = Qt.ArrowCursor;
        if( gameView.running ) cursorTimer.restart();
    }

}
