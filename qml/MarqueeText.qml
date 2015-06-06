import QtQuick 2.4

Item {
    property string text: "Error!"
    property color textColor: "black"
    property int textSeparation: 40
    property int heightPadding : 10
    property int fontSize : 11
    property bool mousedOrSelected: false
    property int animationInterval : 100
    property int animationDelta: 10
    property int animationDuration: animationInterval + 50

    // Will the rendered text fit completely inside its box?
    property bool fitsInsideParent: marqueeTextDummy.width < parent.width

    // Range: 0 to parent.width
    // Does not apply to narrow text
    property int textDisplacement: 0

    // 123, 231, or 312
    // State represents the leftmost text
    // Leftmost text always not visible for the duration of the state
    // Doesn't matter how it looks during this period
    // Current center text is ( state + 1 ) % 3
    property int state: 3

    width: parent.width
    height: marqueeTextDummy.height + heightPadding
    clip: true

    Timer {
        id: animationTimer
        interval: parent.animationInterval
        onTriggered: moveMarquee()
        repeat: true

        // Only scroll if this text is wider than its parent
        running: fitsInsideParent ? false : parent.mousedOrSelected
    }

    function moveMarquee() {

        // No more ...
        makeTextNatural()

        textDisplacement += animationDelta

        switch( state ) {
        case 1:
            marqueeTextOne.visible = false
            marqueeTextTwo.visible = true
            marqueeTextThree.visible = true
            marqueeTextOne.x = marqueeTextDummy.width
            marqueeTextTwo.x = - textDisplacement
            marqueeTextThree.x = marqueeTextDummy.width - textDisplacement + textSeparation
            break
        case 2:
            marqueeTextTwo.visible = false
            marqueeTextThree.visible = true
            marqueeTextOne.visible = true
            marqueeTextTwo.x = marqueeTextDummy.width
            marqueeTextThree.x = - textDisplacement
            marqueeTextOne.x = marqueeTextDummy.width - textDisplacement + textSeparation
            break
        case 3:
            marqueeTextThree.visible = false
            marqueeTextOne.visible = true
            marqueeTextTwo.visible = true
            marqueeTextThree.x = marqueeTextDummy.width
            marqueeTextOne.x = - textDisplacement
            marqueeTextTwo.x = marqueeTextDummy.width - textDisplacement + textSeparation
            break
        default:
            break
        }

        // Time for a state change?
        if( textDisplacement > marqueeTextDummy.width + textSeparation ) {
            state = ( state % 3 ) + 1
            textDisplacement = 0
        }

    }

    Component.onCompleted: onMousedOrSelectedChangedHandler()

    // Wait for animation to finish before adding ...
    // Start timer when mousedOrSelected is changed
    // and its onChanged handler changes fitsInsideParent implicitly
    Timer {
        id: marqueeTimer
        interval: animationDuration
        onTriggered: makeTextClipped()
        repeat: false
        running: !fitsInsideParent
    }

    // Make text width as wide as the text actually is
    function makeTextNatural() {
        marqueeTextOne.width = marqueeTextDummy.width
        marqueeTextTwo.width = marqueeTextDummy.width
        marqueeTextThree.width = marqueeTextDummy.width
    }

    // Make text width the width of the parent
    function makeTextClipped() {
        marqueeTextOne.width = parent.width
        marqueeTextTwo.width = parent.width
        marqueeTextThree.width = parent.width
    }

    onWidthChanged: {
        widthChangedHandler()
    }

    // Sometimes onWidthChanged is not fired
    Timer {
        interval: 1000
        repeat: true
        running: !mousedOrSelected
        onTriggered: widthChangedHandler()
    }

    function widthChangedHandler() {
        if( fitsInsideParent ) {
            state = 1
            marqueeTextOne.visible = false
            marqueeTextTwo.visible = true
            marqueeTextThree.visible = false
            makeTextNatural()
            marqueeTextOne.x = 0
            state = 2
            marqueeTextTwo.x = marqueeTextDummy.width
            state = 3
            marqueeTextThree.x = -marqueeTextDummy.width
        }
        // Reset to state 3
        else {
            state = 1
            marqueeTextThree.visible = false
            marqueeTextOne.visible = true
            marqueeTextTwo.visible = false
            makeTextClipped()
            marqueeTextOne.x = 0
            state = 2
            marqueeTextTwo.x = marqueeTextDummy.width
            state = 3
            marqueeTextThree.x = marqueeTextDummy.width
        }
        textDisplacement = 0

        // Can't hurt
        onMousedOrSelectedChangedHandler()
    }    

    onMousedOrSelectedChanged: {
        makeTextNatural()
        onMousedOrSelectedChangedHandler()
    }

    // Deal with the transition invoked when the mouse is no longer
    // over the item or the item is no longer selected
    function onMousedOrSelectedChangedHandler() {

        // Text only stays horizontally centered when it's its natural size
        if( fitsInsideParent ) makeTextNatural()

        // Resize center text when not moused over or selected and only if wider
        // than parent to parent's width so elipses will get drawn
        else if ( !mousedOrSelected ) {
            marqueeTimer.restart()
            // Follow the shortest path back to left side
            if( textDisplacement > marqueeTextDummy.width / 2 ){
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
                default:
                    break
                }
                textDisplacement = 0
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
                default:
                    break
                }
                textDisplacement = 0
            }
        }
    }

    Text {
        id: marqueeTextOne
        color: parent.textColor
        text: parent.text
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: fitsInsideParent ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: fontSize
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.mousedOrSelected ? Text.ElideRight : Text.ElideRight

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
        visible: !fitsInsideParent
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: fitsInsideParent ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: fontSize
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.mousedOrSelected ? Text.ElideRight : Text.ElideRight

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
        visible: !fitsInsideParent
        anchors {
            verticalCenter: parent.verticalCenter
            // Unlock the anchor if wider than the viewport so it'll stick to the left ( x = 0 )
            horizontalCenter: fitsInsideParent ? parent.horizontalCenter : undefined
        }
        font {
            bold: true
            pixelSize: fontSize
            family: "Sans"
        }
        renderType: Text.QtRendering

        // Shouldn't be necessary, but won't work right without the check
        elide: parent.mousedOrSelected ? Text.ElideRight : Text.ElideRight

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
            pixelSize: fontSize
            family: "Sans"
        }
        renderType: Text.QtRendering
    }
}
