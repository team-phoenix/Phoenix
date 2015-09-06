import QtQuick.Controls.Styles 1.2
import QtQuick.Controls 1.2
import QtQuick 2.4

import vg.phoenix.themes 1.0

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

    height: 25;
    width: 100;
    style: ComboBoxStyle {
        background: Item {
            implicitWidth: control.width;
            implicitHeight: control.height;


            Rectangle {
                id: mainBackground;
                radius: 3;
                anchors.fill: parent;
                color: "#282420";
                border {
                    width: 1;
                    color: "#0a0a0a";
                }

            }

            Rectangle {
                radius: mainBackground.radius;
                anchors {
                    fill: mainBackground;
                    bottomMargin: -1;
                }
                z: mainBackground.z - 1;
                color: "white";
                opacity: 0.1;
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

