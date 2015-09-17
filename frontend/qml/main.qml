import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

ApplicationWindow {
    id: root;
    visible: true;
    visibility: Window.Windowed;
    width: Screen.width * 0.7;
    height: Screen.height * 0.7;
    title: qsTr("Phoenix");
    color: "black";

    minimumHeight: 480;
    minimumWidth: 640;

    property InputManager inputManager: InputManager { gamepadControlsFrontend: true; }

    MouseArea {
        id: rootMouseArea;
        anchors.fill: parent;
        hoverEnabled: true;
        propagateComposedEvents: true;
        z: parent.z + 1;
    }

    StackView {
        id: layoutStackView;
        anchors.fill: parent;
        Component.onCompleted: {
            push( gameView );
            push( { item: mouseDrivenView, properties: { opacity: 0 } } );
        }

        // Here we define the (push) transition that occurs when going from GameView into the library
        Component {
            id: libraryTransition;
            StackViewTransition {
                PropertyAnimation {
                    target: exitItem; property: "opacity";
                    from: 1; to: 0;
                    duration: 1000;
                }

                PropertyAnimation {
                    target: exitItem; property: "scale";
                    from: 1; to: 0.75;
                    duration: 1000;
                    easing.type: Easing.InOutQuad;
                }

                PropertyAnimation {
                    target: enterItem; property: "opacity";
                    from: 0; to: 1;
                    duration: 1000;
                }
            }
        }

        // Here we define the opposite (pop) transition
        Component {
            id: gameTransition;
            StackViewTransition {
                PropertyAnimation {
                    target: exitItem; property: "opacity";
                    from: 1; to: 0;
                    duration: 500;
                }

                PropertyAnimation {
                    target: enterItem; property: "opacity";
                    from: 0; to: 1;
                    duration: 2500;
                    easing.type: Easing.InOutQuad;
                }

                PropertyAnimation {
                    target: enterItem; property: "scale";
                    from: 0.75; to: 1;
                    duration: 2500;
                    easing.type: Easing.InOutQuad;
                }
            }
        }

        delegate: StackViewDelegate {
            pushTransition: libraryTransition;
            popTransition: gameTransition;
        }
    }

    // Debug button to switch between mouse driven and big picture modes
//    Rectangle {
//        anchors {
//            right: parent.right;
//            top: parent.top;

//        }
//        height: 25;
//        width: 25;
//        color: "red"

//        MouseArea {
//            anchors.fill: parent;
//            onClicked: {
//                if ( layoutStackView.currentItem.objectName === "MouseDrivenView" ) {
//                    layoutStackView.push( { item: bigPictureView, replace: true } );
//                } else if ( layoutStackView.currentItem.objectName === "BigPictureView" ) {
//                    layoutStackView.push( { item: mouseDrivenView, replace: true } );
//                }
//            }
//        }
//    }

    Component {
        id: bigPictureView;

        BigPictureView {
            id: pictureView;
            objectName: "BigPictureView";

            Connections {
                target: root.inputManager;
                onDeviceAdded: {
                    console.log( device.name );
                    console.log( device.editMode )
                    device.inputDeviceEvent.connect( pictureView.qmlInputDevice.insert );
                }
            }

            Component.onCompleted: {
                root.inputManager.emitConnectedDevices();
            }
        }
    }

    Component {
        id: mouseDrivenView;

        MouseDrivenView { objectName: "MouseDrivenView"; }
    }

    Component {
        id: gameView;

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
    }
}
