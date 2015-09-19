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
        acceptedButtons: Qt.AllButtons;
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
                    duration: 1000;
                    easing.type: Easing.InOutQuad;
                }

                PropertyAnimation {
                    target: enterItem; property: "scale";
                    from: 0.75; to: 1;
                    duration: 1000;
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

        GameView {

        }
    }
}
