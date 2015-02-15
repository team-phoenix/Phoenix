import QtQuick 2.4

Item {
    width: parent.width;
    height: marqueeText.height + padding;
    clip: true;

    property string text: "hello world"
    property int padding : 10
    property int fontSize : 11
    property int interval : 100
    property color textColor: "black"
    property bool running: true;

    Timer {
        id: timer;
        interval: parent.interval
        onTriggered: parent.moveMarquee()
        running: parent.running;
        repeat: true;
    }

    function moveMarquee()
    {
        if(marqueeText.x + marqueeText.width < 0)
        {
            marqueeText.x = marqueeText.parent.width;
        }
        marqueeText.x -= 1;
    }

    Text {
        id: marqueeText
        anchors.verticalCenter: parent.verticalCenter
        color: parent.textColor
        text: parent.text
        x: parent.width
        width: parent.width;
        anchors {
            horizontalCenter: running ? undefined : parent.horizontalCenter;
        }
        font {
            bold: true;
            pixelSize: fontSize;
            family: "Sans";
        }
        renderType: Text.QtRendering;
        horizontalAlignment: Text.AlignHCenter;
        elide: parent.running ? Text.ElideNone : Text.ElideRight;
    }


}
