import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

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
        color: "transparent";
        height: 32;

        width: parent.width;

        Text {
            anchors {
                left: parent.left;
                verticalCenter: parent.verticalCenter;
            }

            text: inputMappingColumn.headerText;
            font { pixelSize: PhxTheme.common.baseFontSize; }
        }
    }

    Timer {
        id: mappingCollsionTimer;
        interval: 400;
        onTriggered: {
            inputMappingColumn.currentItem.mappingCollisionDetected = false;
        }
    }


    delegate: Item {
        id: columnItem;
        height: 32;
        width: 100;

        anchors {
            left: parent.left;
            right: parent.right;
        }

        property bool mappingCollisionDetected: false;

        property string text: inputSettingsView.currentMapping === undefined
                              ? "None" : inputSettingsView.currentMapping[ key ];

        function handleEvent( event, state, type ) {
            if ( state ) {
                if ( root.inputManager.get( devicesCombobox.currentText ).setMappings( key, event, type ) ) {
                    inputMappingColumn.currentIndex = -1;
                    root.inputManager.get( devicesCombobox.currentText ).editModeEvent.disconnect( handleEvent );
                    inputSettingsView.currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
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
            spacing: 6;

            Rectangle {
                radius: 5;
                color: "transparent";

                Layout.fillHeight: true;
                Layout.fillWidth: true;

                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    //leftMargin: 12;
                }

                Text {
                    id: displayText;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        right: parent.right;
                    }

                    text: displayButton;
                    color: "white";
                }
            }
                //color: columnItem.mappingCollisionDetected ? "red" : "#2e1510";

            PhxSearchBar {
                id: textField;
                width: 100;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }

                text: columnItem.text;
            }
        }
    }
}
