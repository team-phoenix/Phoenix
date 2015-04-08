import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {

    // This component is just two dropshadows around some text.
    // This is used in the ConsoleBar and is shown by the 'Library'
    // and the 'Collections' text.

    id: shadowedText;
    height: label.height;
    width: label.width;
    property string text: "";
    property int pixelSize: 10;
    property color color: "#f1f1f1";

    DropShadow {
        anchors.fill: source;
        source: blackShadow;
        color: "white";
        opacity: 0.3;
        verticalOffset: 1;
        horizontalOffset: 0;
        radius: 1;
        samples: radius * 2;
    }

    DropShadow {
        id: blackShadow;
        anchors.fill: source;
        source: label;
        color: "black";
        verticalOffset: 2;
        horizontalOffset: 0;
        radius: 1;
        samples: radius * 2;
    }

    Text {
        id: label;
        anchors.centerIn: parent;
        renderType: Text.QtRendering;
        text: parent.text;
        color: parent.color;
        opacity: 0.2;
        font {
            bold: true;
            family: "Sans";
            pixelSize: shadowedText.pixelSize;
        }
    }
}
