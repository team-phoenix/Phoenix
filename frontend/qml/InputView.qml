import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.backend 1.0

Rectangle {
    width: 100
    height: 62
    color: "yellow";

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
                headerText: qsTr( "Directional" );

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

                model: ListModel {
                    ListElement { displayButton: "Left Stick"; key: "leftstick"; value: InputDeviceEvent.L3 }
                    ListElement { displayButton: "Right Stick"; key: "rightstick"; value: InputDeviceEvent.R2 }
                    ListElement { displayButton: "Left Shoulder"; key: "leftshoulder"; value: InputDeviceEvent.L }
                    ListElement { displayButton: "Right Shoulder"; key: "rightshoulder"; value: InputDeviceEvent.R }
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

