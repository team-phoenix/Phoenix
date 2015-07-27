import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

import vg.phoenix.backend 1.0

Rectangle {
    id: inputView;
    width: 100
    height: 62
    color: "yellow";

    property ListModel inputDevicesModel: ListModel {
        ListElement { name: ""; port: 0  }

        Component.onCompleted: {
            //clear();
        }
    }

    ExclusiveGroup {
        id: inputColumnGroup;
    }

    function handleDeviceAdded( device ) {

        device.editMode = true;

        console.log( "name: " + device.name + " modelCount: " + devicesCombobox.count );
        inputDevicesModel.append( { "name": device.name  } );

    }


    Component.onDestruction: {

        for ( var i = 0; i < root.inputManager.count; ++i ) {
            if ( root.inputManager.at( i ) ) {
                root.inputManager.at( i ).editMode = false;
            }
        }

    }

    Component.onCompleted: {
        root.inputManager.deviceAdded.connect( handleDeviceAdded );
        root.inputManager.emitConnectedDevices();

    }

    property var currentMapping: undefined;

    Rectangle {
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

        color: "pink";

        ComboBox {
            id: devicesCombobox;
            model: inputView.inputDevicesModel;

            textRole: "name";

            onCurrentTextChanged: {
                console.log( "currenttext: ",  currentText );
                if ( root.inputManager.get( devicesCombobox.currentText ) !== null ) {
                    currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
                }
            }

        }

        RowLayout {
            anchors {
                fill: parent;
            }

            spacing: 24;

            InputMappingColumn {
                id: actionButtonColumn;

                width: 75;

                anchors {
                    left: parent.left;
                }

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
                width: 100;
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
        }

    }
}

