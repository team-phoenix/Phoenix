import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle {
    id: infoBar;
    visible: height !== 0;
    height: (root.infoBarText !== "") ? 40 : 0;
    gradient: Gradient {
        GradientStop {position: 0.0; color: "#e85955";}
        GradientStop {position: 0.2; color: "#e8433f";}
    }

    border {
        width: 1;
        color: "black"
    }

    Text {
        anchors {
            left: parent.left;
            leftMargin: 12;
            verticalCenter: parent.verticalCenter;
        }
        text: root.infoBarText;

        font {
            family: "Sans";
            pixelSize: 14;
            bold: true;
        }
        color: "#262626";
    }



    PhoenixWarningButton {
        visible: infoBar.height !== 0;
        anchors {
            right: parent.right;
            verticalCenter: parent.verticalCenter;
            rightMargin: 25;
        }
        onClicked: infoBar.height = 0;
    }
}
