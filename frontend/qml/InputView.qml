import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

Item {
    id: inputView;
    width: 100
    height: 62


    property ListModel inputDevicesModel: ListModel {
        ListElement { name: ""; port: 0  }

        Component.onCompleted: {
            clear()
            root.inputManager.deviceAdded.connect( handleDeviceAdded );
            root.inputManager.emitConnectedDevices();
        }

        Component.onDestruction: {
            root.inputManager.deviceAdded.disconnect( handleDeviceAdded );
            for ( var i = 0; i < root.inputManager.count; ++i ) {
                if ( root.inputManager.at( i ) ) {
                    root.inputManager.at( i ).editMode = false;
                    root.inputManager.at( i ).editModeEvent.disconnect();
                }
            }
        }

    }

    ExclusiveGroup {
        id: inputColumnGroup;
    }

    function handleDeviceAdded( device ) {

        device.editMode = true;

        if (  inputDevicesModel.count > 0 && inputDevicesModel.get( 0 ).name === "" ) {
            inputDevicesModel.set( 0, { "name": device.name } );
        } else {
            inputDevicesModel.append( { "name": device.name  } );
        }
    }

    // This is a QVariantMap that is used to hold the currently selected device's mapping.
    property var currentMapping: undefined;

    Item {
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
            bottomMargin: 56;
            topMargin: 84;
            leftMargin: 56;
            rightMargin: 56;
        }

        DropShadow {
            id: controllerShadow;
            anchors.fill: source;
            source: controllerImage;
            verticalOffset: 1;
            horizontalOffset: 0;
            color: "black";
            transparentBorder: true;
            radius: 8;
            samples: radius * 2;
        }

        Image {
            id: controllerImage;
            anchors {
                bottom: parent.bottom;
                bottomMargin: -100;
                right: inputScrollView.left;
                rightMargin: 24;
                left: parent.left;
                top: parent.top;
                topMargin: 100;
            }

            height: 400;

            source: "playstationController.svg";
            fillMode: Image.PreserveAspectFit;

            sourceSize { height: height; width: width; }

        }

        ScrollView {
            id: inputScrollView;
            anchors {
                right: parent.right;
                rightMargin: 12;
                top: parent.top;
                bottom: parent.bottom;
            }

            width: 250;

            ColumnLayout {
                spacing: 6;

                PhxComboBox {
                    id: inputDeviceComboBox;
                    anchors {
                        right: parent.right;
                    }
                }

                PhxComboBox {
                    anchors {
                        right: parent.right;
                    }
                    model: [ "Player 1" ];
                }

                InputMappingColumn {
                    id: actionButtonColumn;

                    Layout.fillWidth: true;

                    height: 175;

                    headerText: qsTr( "Actions" );
                    exclusiveGroup: inputColumnGroup;
                    checked: false;

                    onExclusiveGroupChanged: {
                        if ( exclusiveGroup ) {
                            exclusiveGroup.bindCheckable( actionButtonColumn );
                        }
                    }

                    model: ListModel {
                        ListElement { displayButton: "A"; key: "a"; value: InputDeviceEvent.A }
                        ListElement { displayButton: "B"; key: "b"; value: InputDeviceEvent.B }
                        ListElement { displayButton: "X"; key: "x"; value: InputDeviceEvent.X }
                        ListElement { displayButton: "Y"; key: "y"; value: InputDeviceEvent.Y }
                    }
                }

                InputMappingColumn {
                    id: dpadButtonColumn;
                    Layout.fillWidth: true;
                    height: 175;
                    headerText: qsTr( "Digital" );
                    exclusiveGroup: inputColumnGroup;
                    checked: false;

                    onExclusiveGroupChanged: {
                        if ( exclusiveGroup ) {
                            exclusiveGroup.bindCheckable( dpadButtonColumn );
                        }
                    }

                    model: ListModel {
                        ListElement { displayButton: "Up"; key: "dpup"; value: InputDeviceEvent.Up }
                        ListElement { displayButton: "Left"; key: "dpleft"; value: InputDeviceEvent.Left }
                        ListElement { displayButton: "Right"; key: "dpright"; value: InputDeviceEvent.Right }
                        ListElement { displayButton: "Down"; key: "dpdown"; value: InputDeviceEvent.Down }
                    }
                }

                InputMappingColumn {
                    id: miscButtonColumn;

                    Layout.fillWidth: true;

                    height: 250;

                    headerText: qsTr( "Misc." );
                    exclusiveGroup: inputColumnGroup;
                    checked: false;

                    onExclusiveGroupChanged: {
                        if ( exclusiveGroup ) {
                            exclusiveGroup.bindCheckable( miscButtonColumn );
                        }
                    }
                    model: ListModel {
                        ListElement { displayButton: "L3"; key: "leftstick"; value: InputDeviceEvent.L3 }
                        ListElement { displayButton: "R3"; key: "rightstick"; value: InputDeviceEvent.R2 }
                        ListElement { displayButton: "L"; key: "leftshoulder"; value: InputDeviceEvent.L }
                        ListElement { displayButton: "R"; key: "rightshoulder"; value: InputDeviceEvent.R }
                        ListElement { displayButton: "Start"; key: "start"; value: InputDeviceEvent.Start }
                        ListElement { displayButton: "Select"; key: "back"; value: InputDeviceEvent.Select }
                    }
                }

                /*
                InputMappingColumn {
                    id: analogColumn;
                    headerText: qsTr( "Analog" );

                    anchors {
                        right: parent.right;
                    }
                    exclusiveGroup: inputColumnGroup;
                    checked: false;

                    onExclusiveGroupChanged: {
                        if ( exclusiveGroup ) {
                            exclusiveGroup.bindCheckable( analogColumn );
                        }
                    }

                    model: ListModel {
                        ListElement { displayButton: "Up"; key: "dpup"; value: InputDeviceEvent.Up }
                        ListElement { displayButton: "Left"; key: "dpleft"; value: InputDeviceEvent.Left }
                        ListElement { displayButton: "Right"; key: "dpright"; value: InputDeviceEvent.Right }
                        ListElement { displayButton: "Down"; key: "dpdown"; value: InputDeviceEvent.Down }
                    }
                }
                */
            }
        }

    }
}
