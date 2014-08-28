import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    color: settingsBubble.contentColor;

    Column {
        visible: stackView.width > width;
        anchors {
            fill: parent;
            topMargin: 25;
        }
        spacing: 15;

        CheckBox {
            id: debugCheckBox;
            anchors.horizontalCenter: parent.horizontalCenter;
            text: "Debug Mode";
            checked: false;
        }

        GroupBox {
            id: groupBox;
            enabled: debugCheckBox.checked;
            anchors.horizontalCenter: parent.horizontalCenter;
            height: 400;
            width: parent.width * 0.85;
            Rectangle {
                anchors.fill: parent;
                color: debugCheckBox.checked ? "darkgray" : "gray" ;

                Column {
                    anchors.fill: parent;
                    Button {
                        text: "Button";
                    }
                }
            }
        }
    }
}
