import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

ApplicationWindow {
    id: root;
    visible: true
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
        initialItem: mouseDrivenView;
    }


    Rectangle {
        anchors {
            right: parent.right;
            top: parent.top;

        }
        height: 25;
        width: 25;
        color: "red"

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                if ( layoutStackView.currentItem.objectName === "MouseDrivenView" ) {
                    layoutStackView.push( { item: bigPictureView, replace: true } );
                } else if ( layoutStackView.currentItem.objectName === "BigPictureView" ) {
                    layoutStackView.push( { item: mouseDrivenView, replace: true } );
                }
            }
        }
    }

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




}
