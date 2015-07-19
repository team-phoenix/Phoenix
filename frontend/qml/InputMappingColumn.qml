import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.backend 1.0

ListView {
    id: inputGridView;

    height: 300;
    width: 200;

    model: undefined;

    orientation: ListView.Vertical;
    spacing: 0;

    property string headerText: "";

    header: Rectangle {
        color: "lightgray";
        height: 32;

        width: parent.width;

        Text {
            anchors.centerIn: parent;
            text: inputGridView.headerText;
            font {
                pixelSize: 13;
            }
        }
    }

    delegate: Item {

        height: 32;
        width: parent.width;

        RowLayout {
            anchors.fill: parent;
            spacing: 12;

            Rectangle {

                color: "blue";
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
                color: "green";
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                }

                Text {
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        right: parent.right;
                        rightMargin: 12;
                    }

                    text: value;
                    color: "white";
                }
            }
        }
    }
}
