import QtQuick 2.5
import QtQuick.Window 2.0

Window {
    id: dropDown;
    height: 200;
    width: 125;

    flags: Qt.Popup | Qt.FramelessWindowHint;
    color: "transparent";

    function popup( x, y ) {
        dropDown.x = root.x + x -  ( dropDown.width / 2 );
        dropDown.y = root.y + y + ( 12 );
        dropDown.visible = true;
    }

    Rectangle {
        anchors {
            fill: parent;
            topMargin: 13;
            bottomMargin: 25;
        }
        radius: 6;

        Rectangle {
            rotation: 45;
            color: parent.color;
            anchors {
                top: parent.top;
                topMargin: -height / 2;
                horizontalCenter: parent.horizontalCenter;
            }
            height: 13;
            width: height;
        }
    }
}
