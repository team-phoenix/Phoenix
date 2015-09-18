import QtQuick 2.5
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

Rectangle {
    color: "black";

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
                if( run === true ) {
                    rootMouseArea.cursorShape = Qt.ArrowCursor;
                    videoItemContainer.opacity = 1.0;
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
}
