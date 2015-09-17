import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

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

    property InputManager inputManager: InputManager {
        gamepadControlsFrontend: true;

    }

    StackView {
        id: layoutStackView;
        anchors.fill: parent;
        initialItem: mouseDrivenView;
        delegate: StackViewDelegate {
            replaceTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem; property: "opacity";
                    from: 0; to: 1;
                    duration: 500;
                }
                PropertyAnimation {
                    target: exitItem; property: "opacity";
                    from: 1; to: 0;
                    duration: 500;
                }
            }
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

        MouseDrivenView {

            objectName: "MouseDrivenView";
        }
    }

    Component {
        id: gameView;

        Rectangle {
            id: videoItemContainer;
            anchors.fill: parent;
            color: "black";

            // A small workaround to guarantee that the core and game are loaded in the correct order
            property var coreGamePair: [ "", "" ];
            onCoreGamePairChanged: {
                console.log( "coreGamePair changed to: " + coreGamePair );
                if( coreGamePair !== [ "", "" ] ) {
                    videoItem.libretroCore = coreGamePair[ 0 ];
                    videoItem.game = coreGamePair[ 1 ];
                }
            }

            // A blurred copy of the video that sits behind the real video as an effect
            FastBlur {
                id: blurEffect;
                anchors.fill: parent;
                source: videoItem;
                radius: 64;
                rotation: 180;
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
            VideoItem {
                id: videoItem;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    horizontalCenter: parent.horizontalCenter;
                }

                width: height * aspectRatio;

                onAspectRatioChanged: {
                    console.log( "Aspect ratio: " + aspectRatio );
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
