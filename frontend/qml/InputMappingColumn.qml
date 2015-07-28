import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.backend 1.0

ListView {
    id: inputMappingColumn;

    height: 300;
    width: 200;

    model: undefined;

    interactive: false;

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

    Timer {
        id: mappingCollsionTimer;
        interval: 400;
        onTriggered: {
            inputMappingColumn.currentItem.mappingCollisionDetected = false;
        }
    }


    delegate: Rectangle {
        id: columnItem;
        height: 32;
        width: 100;

        anchors {
            left: parent.left;
            right: parent.right;
        }

        color: "#82342e";
        radius: 5;

        property bool mappingCollisionDetected: false;

        property string text: inputView.currentMapping === undefined
                              ? "None" : inputView.currentMapping[ key ];

        function handleEvent( event, state, type ) {
            if ( state ) {
                if ( root.inputManager.get( devicesCombobox.currentText ).setMappings( key, event, type ) ) {
                    inputMappingColumn.currentIndex = -1;
                    root.inputManager.get( devicesCombobox.currentText ).editModeEvent.disconnect( handleEvent );
                    inputView.currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
                } else {
                    mappingCollisionDetected = true;
                    mappingCollsionTimer.start();
                    console.log( "Collsion QML Detected." );
                }
            }
        }

        focus: checked;

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                mappingCollsionTimer.stop();

                inputMappingColumn.checked = true;

                if ( index !== inputMappingColumn.currentIndex ) {
                    if ( inputMappingColumn.currentIndex > -1 ) {
                        root.inputManager.get( devicesCombobox.currentText ).editModeEvent.disconnect( inputMappingColumn.currentItem.handleEvent );
                    }
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
                color: columnItem.mappingCollisionDetected ? "red" : "#2e1510";
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }

                border {
                    color: "orange";
                    width: checked && index == inputMappingColumn.currentIndex ? 6 : 0;
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
