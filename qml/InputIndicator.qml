import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {
    id: inputIndicator;
    visible: false;
    property Timer timer: Timer {
        id: timer;
        interval: 2000;
        onTriggered: {
            inputIndicator.visible = false;
        }
    }



    Connections {
        target: inputmanager;
        onCountChanged: {
            inputIndicator.visible = true;
            inputIndicator.timer.start();
        }
    }




    /*DropShadow {
        id: dropShadow;
        visible: parent.visible;
        anchors.fill: controllerUpdate;
        source: controllerUpdate;
        color: "black";
        samples: 8;
        radius: 16;
        verticalOffset: 1;
        horizontalOffset: 0;

    }*/



    Rectangle {
        id: controllerUpdate;
        //visible: ;
        anchors.fill: parent;
        radius: parent.height / 2;
        border {
            width: 1;
            color: "black";
        }

        Text {
            z: 101;
            anchors.centerIn: parent;
            text: inputmanager.count;
            renderType: Text.QtRendering;
            font {
                family: "Sans";
                pixelSize: 16;
                bold: true;
            }
            color: "black";
        }

        gradient: Gradient {
            GradientStop {color: "#fb7934"; position: 0.0}
            GradientStop {color: "#e9163a"; position: 1.0}
        }

    }
}
