import QtQuick 2.4
import QtQuick.Layouts 1.1

import vg.phoenix.themes 1.0;

RowLayout {
    height: 50;
    width: 125;
    spacing: 0;

    Rectangle {
        anchors {
            bottom: parent.bottom;
            bottomMargin: -height;
            left: parent.left;
            right: parent.right;
        }

        height: 3;
        color: "white";
        opacity: 0.05;

    }

    Rectangle {
        color: "transparent";
        height: parent.height;
        width: 125;

        Text {
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 12;
            }
            color: PhxTheme.common.baseFontColor;
            text:  "4";
            font {
                pixelSize: 22;
                bold: true;
            }
        }

    }

    Rectangle {
        Layout.fillHeight: true;
        width: 3;
        opacity: 0.05;

        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent"; }
            GradientStop { position: 0.9; color: "white"; }
            GradientStop { position: 1.0; color: "white"; }
        }
    }

    Rectangle {
        color: "transparent";
        height: parent.height;
        Layout.fillWidth: true;

        Text {
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 12;
            }

            color: PhxTheme.common.baseFontColor;
            text: qsTr( "7:25 AM" );
        }
    }
}

