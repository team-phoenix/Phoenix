import QtQuick 2.4

Item {
    property string text: "Error!"
    property int padding : 10
    property int fontSize : 11
    property int interval : 100
    property color textColor: "black"
    property bool running: false
    property int textSeparation: 40
    property int delta: 10
    property int animationDuration: 200

    // Will the rendered text fit completely inside its box?
    property bool narrow: marqueeTextDummy.width < parent.width

    // Range: 0 to parent.width
    // Does not apply to narrow text
    property int displacement: 0

    // 123, 231, or 312
    // State represents the leftmost text
    // Current center text is ( state + 1 ) % 3
    property int state: 3

    // Keep the parent's width constantly checked so this Item's width changes as the grid gets resized
    width: parent.width
    height: marqueeTextDummy.height + padding
    clip: true

    Timer {
        id: timer
        interval: parent.interval
        onTriggered: moveMarquee()
        repeat: true

        // Only scroll if this text is wider than its parent
        running: narrow ? false : parent.running
    }

    function moveMarquee() {

        displacement += delta

        switch( state ) {
        case 1:
            marqueeTextOne.x = marqueeTextDummy.width
            marqueeTextTwo.x = - displacement
            marqueeTextThree.x = marqueeTextDummy.width - displacement + textSeparation
            break
        case 2:
            marqueeTextTwo.x = marqueeTextDummy.width
            marqueeTextThree.x = - displacement
            marqueeTextOne.x = marqueeTextDummy.width - displacement + textSeparation
            break
        case 3:
            marqueeTextThree.x = marqueeTextDummy.width
            marqueeTextOne.x = - displacement
            marqueeTextTwo.x = marqueeTextDummy.width - displacement + textSeparation
            break
        default:
            break
        }

        // Time for a state change
        if( displacement > marqueeTextDummy.width + textSeparation ) {
            state = ( state % 3 ) + 1
            displacement = 0;
        }

    }

    Component.onCompleted: recalculate()

    // Wait for animation to finish before adding ...
    Timer {
        id: marqueeTimer
        interval: animationDuration
        onTriggered: {
            marqueeTextThree.width = parent.width
            marqueeTextOne.width = parent.width
            marqueeTextTwo.width = parent.width
        }
        repeat: false
        running: !narrow
    }

    function recalculate() {
        if( narrow ) {
            marqueeTextOne.width = marqueeTextDummy.width
            marqueeTextTwo.width = marqueeTextDummy.width
            marqueeTextThree.width = marqueeTextDummy.width
        }

        // Resize center text when not moused over and only if wider
        // than parent to parent's width so elipses will get drawn
        else if ( !running ) {
            marqueeTimer.restart()
            // Follow the shortest path back to left side
            if( displacement > marqueeTextDummy.width / 2 ){
                switch( state ) {
                case 1:
                    marqueeTextOne.x = marqueeTextDummy.width
                    marqueeTextTwo.x = -marqueeTextDummy.width
                    marqueeTextThree.x = 0
                    break
                case 2:
                    marqueeTextTwo.x = marqueeTextDummy.width
                    marqueeTextThree.x = -marqueeTextDummy.width
                    marqueeTextOne.x = 0
                    break
                case 3:
                    marqueeTextThree.x = marqueeTextDummy.width
                    marqueeTextOne.x = -marqueeTextDummy.width
                    marqueeTextTwo.x = 0
                    break
                }
                displacement = 0
                state = ( state % 3 ) + 1
            }
            else {
                switch( state ) {
                case 1:
                    marqueeTextOne.x = marqueeTextDummy.width
                    marqueeTextTwo.x = 0
                    marqueeTextThree.x = marqueeTextDummy.width
                    break
                case 2:
                    marqueeTextTwo.x = marqueeTextDummy.width
                    marqueeTextThree.x = 0
                    marqueeTextOne.x = marqueeTextDummy.width
                    break
                case 3:
                    marqueeTextThree.x = marqueeTextDummy.width
                    marqueeTextOne.x = 0
                    marqueeTextTwo.x = marqueeTextDummy.width
                    break
                }
                displacement = 0
            }
        }
    }

    onRunningChanged: {
        marqueeTextOne.width = marqueeTextDummy.width
        marqueeTextTwo.width = marqueeTextDummy.width
        marqueeTextThree.width = marqueeTextDummy.width
        marqueeTimer.interval = animationDuration
        recalculate()
    }
    onWidthChanged: {
        marqueeTimer.interval = 5
        recalculate()
    }
    Text {
        id: marqueeTextOne
        color: parent.textColor
        text: parent.text
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: narrow ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: 0
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.running ? Text.ElideRight : Text.ElideRight

        Behavior on x {
            PropertyAnimation {
                easing {
                    type: Easing.Linear
                }
                // Once marqueeTextDuplicate occupies the same position as the original
                // marqueeText did ( x = 0 ), don't smooth the animation anymore teleport the text back
                duration: state == 1 ? 0 : animationDuration
            }
        }
    }
    Text {
        id: marqueeTextTwo
        color: parent.textColor
        text: parent.text
        x: marqueeTextDummy.width
        visible: !narrow
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: narrow ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: 0
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.running ? Text.ElideRight : Text.ElideRight

        Behavior on x {
            PropertyAnimation {
                easing {
                    type: Easing.Linear
                }
                // Once marqueeTextDuplicate occupies the same position as the original
                // marqueeText did ( x = 0 ), don't smooth the animation anymore teleport the text back
                duration: state == 2 ? 0 : animationDuration
            }
        }
    }
    Text {
        id: marqueeTextThree
        color: parent.textColor
        text: parent.text
        x: marqueeTextDummy.width
        visible: !narrow
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: narrow ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: 0
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.running ? Text.ElideRight : Text.ElideRight

        Behavior on x {
            PropertyAnimation {
                easing {
                    type: Easing.Linear
                }
                // Once marqueeTextDuplicate occupies the same position as the original
                // marqueeText did ( x = 0 ), don't smooth the animation anymore teleport the text back
                duration: state == 3 ? 0 : animationDuration
            }
        }
    }

    Text {
        id: marqueeTextDummy
        text: parent.text
        visible: false
        font {
            bold: true
            pixelSize: 0
            family: "Sans"
        }
        renderType: Text.QtRendering
    }
}
