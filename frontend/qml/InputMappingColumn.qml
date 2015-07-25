import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.backend 1.0

ListView {
    id: inputMappingColumn;

    height: 300;
    width: 200;

    model: undefined;


    property bool checked: false;
    property ExclusiveGroup exclusiveGroup: null;

    onCheckedChanged: {
        if ( !checked ) {
            root.inputManager.get( devicesCombobox.currentText ).editModeEvent.disconnect( inputMappingColumn.currentItem.handleEvent );
        } else {
            inputMappingColumn.forceActiveFocus();
        }
    }

    orientation: ListView.Vertical;
    spacing: 3;

    property string headerText: "";

    header: Rectangle {
        color: "lightgray";
        height: 32;

        width: parent.width;

        Text {
            anchors.centerIn: parent;
            text: inputMappingColumn.headerText;
            font {
                pixelSize: 13;
            }
        }
    }

    delegate: Rectangle {
        id: columnItem;
        height: 32;
        width: 100;

        color: "#82342e";
        radius: 5;


        property string text: inputView.currentMapping === undefined
                              ? "" : inputView.currentMapping[ key ]//value;

        function handleEvent( event, state, type ) {
            if ( state ) {
                root.inputManager.get( devicesCombobox.currentText ).setMappings( key, event, type );
                inputView.currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
            }

        }

        focus: checked;

        Keys.onPressed: {
            if ( devicesCombobox.currentText === "Keyboard" ) {
                root.inputManager.get( devicesCombobox.currentText ).setMappings( key, event.key, InputDeviceEvent.ButtonEvent );
                inputView.currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                inputMappingColumn.checked = true;
                if ( index !== inputMappingColumn.currentIndex ) {
                    root.inputManager.get( devicesCombobox.currentText ).editModeEvent.disconnect( inputMappingColumn.currentItem.handleEvent );
                    inputMappingColumn.currentIndex = index;
                    root.inputManager.get( devicesCombobox.currentText ).editModeEvent.connect( columnItem.handleEvent );
                } else {
                    root.inputManager.get( devicesCombobox.currentText ).editModeEvent.connect( columnItem.handleEvent );
                }
            }
        }

        RowLayout {
            anchors.fill: parent;
            spacing: 12;

            Rectangle {
                radius: 5;
                color: "transparent";
                Layout.fillHeight: true;
                width: displayText.width + 21;

                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    //leftMargin: 12;
                }

                Text {
                    id: displayText;
                    anchors.centerIn: parent;
                    text: displayButton;
                    color: "white";
                }

            }

            Rectangle {
                Layout.fillHeight: true;
                Layout.fillWidth: true;
                color: "#2e1510";
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }

                Text {
                    id: textField;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        right: parent.right;
                        rightMargin: 12;
                    }

                    text: columnItem.text;
                    color: "white";
                }
            }
        }
    }
}
