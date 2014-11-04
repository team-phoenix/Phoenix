import QtQuick 2.3
import QtQuick.Controls 1.2

Rectangle {
    id: infoBar;
    visible: height !== 0;
    height: 0;
    onHeightChanged: {
        if (height === 0)
            root.infoBarText = "";
    }


    gradient: Gradient {
        GradientStop {position: 0.0; color: "#e85955";}
        GradientStop {position: 0.2; color: "#e8433f";}
    }

    border {
        width: 1;
        color: "black"
    }

    Text {
        id: infoText;
        anchors {
            left: parent.left;
            leftMargin: 12;
            verticalCenter: parent.verticalCenter;
        }
        renderType: Text.QtRendering;
        text: root.infoBarText;
        onTextChanged: {
            if (infoText.text !== "") {
                infoBar.height = 40;
                if (timer.running)
                    timer.restart();
                else
                    timer.start();
            }
        }

        font {
            family: "Sans";
            pixelSize: 12;
            bold: true;
        }
        color: "#262626";
    }

    Timer {
        id: timer;
        interval: 2500;
        running: false;
        onTriggered: infoBar.height = 0;
    }

}
