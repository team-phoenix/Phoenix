import QtQuick 2.4

Item {
    // Make the qml runtime constantly check parent's width so it changes as the grid gets resized
    width: running ? parent.width : parent.width;
    height: marqueeText.height + padding;
    clip: true;

    property string text: "Error!";
    property int padding : 10
    property int fontSize : 11
    property int interval : 100
    property color textColor: "black";
    property bool running: false;
    property int textSeparation: 40;
    property int displacement: 10;

    Timer {
        id: timer;
        interval: parent.interval
        onTriggered: parent.moveMarquee();

        // Only scroll if this text is wider than its parent
        running: marqueeTextDuplicate.width > parent.width ? ( parent.running ) : false;

        repeat: true;
    }

    function moveMarquee() {
        marqueeText.width = marqueeTextDuplicate.width;
        if( ( marqueeText.x + marqueeTextDuplicate.width + textSeparation ) < 0 ) {
            marqueeText.x = 0;
        }
        marqueeText.x -= displacement;
    }

    onRunningChanged: {
        // On mouse exit, make the titles that are wide enough to marquee the same width as their parent so elides work
        if( running == false ){
            if( ( marqueeTextDuplicate.width > parent.width ) ){
                marqueeText.width = parent.width;
                marqueeText.x = 0;
            }
        }
    }
    onWidthChanged: {
        // On resize, recalculate if elides should be drawn, setting it by messing with the text's width
        if( running == false ){
            if( ( marqueeTextDuplicate.width > parent.width ) ){
                marqueeText.width = parent.width;
                marqueeText.x = 0;
            }
            else {
                marqueeText.width = marqueeTextDuplicate.width;
            }
        }
    }

    Text {
        id: marqueeText
        anchors.verticalCenter: parent.verticalCenter
        color: parent.textColor
        text: parent.text
        anchors {
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: marqueeTextDuplicate.width > parent.width ? undefined : parent.horizontalCenter;
        }
        font {
            bold: true;
            pixelSize: 0;
            family: "Sans";
        }
        renderType: Text.QtRendering;

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.running ? Text.ElideRight : Text.ElideRight;

        Behavior on x {
            PropertyAnimation {
                easing {
                    type: Easing.Linear;
                }
                // Once marqueeTextDuplicate occupies the same position as the original
                // marqueeText did ( x = 0 ), don't smooth the animation anymore; teleport the text back
                duration: ( marqueeText.x + marqueeTextDuplicate.width + textSeparation < 0 ) ? 0 : 100;
            }
        }
    }

    Text {
        id: marqueeTextDuplicate
        anchors.verticalCenter: parent.verticalCenter
        color: parent.textColor
        text: parent.text
        x: running ? marqueeText.x + marqueeText.width + textSeparation : parent.width
        visible: ( width > parent.width ) && running
        anchors {
            horizontalCenter: running ? undefined : parent.right;
        }
        font {
            bold: true;
            pixelSize: 0
            family: "Sans";
        }
        renderType: Text.QtRendering;
    }
}
