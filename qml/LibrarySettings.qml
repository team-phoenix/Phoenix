import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    color: settingsBubble.contentColor;

    Row {
        spacing: 2;
        visible: stackView.width > width;
        anchors {
            top: parent.top;
            topMargin: 30;
            horizontalCenter: parent.horizontalCenter;
        }
        Button {
            text: "Import";
            width: 100;
            height: 30;
            style: ButtonStyle {
                background: Rectangle {
                    color: "lightgray";
                }
                label: Label {
                    text: "Import";
                    color: "#f1f1f1";
                    renderType: Text.QtRendering;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                }
            }
        }

        Button {
            text: "Backup";
            width: 100;
            height: 30;
            style: ButtonStyle {
                background: Rectangle {
                    color: "lightgray";
                }
                label: Label {
                    text: "Import";
                    renderType: Text.QtRendering;
                    color: "#f1f1f1";
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                }
            }
        }

        Button {
            text: "Delete";
            width: 100;
            height: 30;
            style: ButtonStyle {
                background: Rectangle {
                    color: root.accentColor;
                }
                label: Label {
                    text: "Delete";
                    color: "#f1f1f1";
                    renderType: Text.QtRendering;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                }
            }
        }
    }
}
