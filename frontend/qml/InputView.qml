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

        ScrollView {
            anchors {
                right: parent.right;
                rightMargin: 12;
                top: parent.top;
                bottom: parent.bottom;
            }

            width: 250;

            ColumnLayout {
                spacing: 0;

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

