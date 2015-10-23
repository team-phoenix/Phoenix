import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import vg.phoenix.themes 1.0

ComboBox {
    id: devicesCombobox;
    model: inputSettingsView.inputDevicesModel;
    textRole: "name";

    onCurrentTextChanged: {
        console.log( "currenttext: ",  currentText );
        if ( root.inputManager.get( devicesCombobox.currentText ) !== null ) {
            currentMapping = root.inputManager.get( devicesCombobox.currentText ).mapping();
        }
    }

    height: 25;
    width: 100;
    style: ComboBoxStyle {
        background: Item {
            implicitWidth: control.width;
            implicitHeight: control.height;

            Rectangle {
                anchors.fill: parent;
                color: "#1a1b20";
                radius: 3;
                border {
                    width: 1;
                    color: "#111216";
                }

                Rectangle {
                    z: parent.z - 1;
                    radius: parent.radius;
                    anchors {
                        fill: parent;
                        bottomMargin: -1;
                    }
                    color: "white";
                    opacity: 0.1;

                }
            }
        }

        label: Item {
            Label {
                anchors {
                    left: parent.left;
                    verticalCenter: parent.verticalCenter;
                    leftMargin: 12;
                }
                text: control.currentText;

                color: PhxTheme.common.baseFontColor;

            }
        }
    }

}

