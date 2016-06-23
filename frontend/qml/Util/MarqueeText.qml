import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.themes 1.0

Item {
    id: marqueeTextRoot;

    // Don't draw text outside of this Item's borders
    clip: true;

    // Use the text's implicit height by default
    // Width must always be explicitly set
    height: text1.height;

    state: "idle";

    // Feel free to change these
    property string text: "Error!";
    property int fontSize: PhxTheme.common.baseFontSize;
    property string fontFamily: PhxTheme.common.systemFontFamily;
    property color color: "white";
    property int spacing: 20;
    property bool running: false;
    property double pixelsPerFrame: 1;
    property bool bold: false;
    property bool forward: true;

    property bool finishedAnimating: true;

    property bool debug: false;

    // Updated on demand, not via a property binding
    property double animationDuration: 0;
    property double animationElapsedTime: 0;
    property double animationRemainingTime: 0;

    property double textWidth: text2.contentWidth;
    property bool needsMarqueeAtAll: textWidth > width;

    property int horizontalAlignment: Text.AlignHCenter;

    // Fully re-evaluate everything once we've finished loading
    Component.onCompleted: handleSituationChanged();

    // Handle the changing width of this Item affecting the need to have the marquee text scroll at all
    onNeedsMarqueeAtAllChanged: handleSituationChanged();

    // Handle the situation when the marquee text loses focus and needs to reset back to its elided position
    // ...or gains focus and needs to have its animation begin
    onRunningChanged: handleSituationChanged();

    // Handle bold toggling affecting width of text
    onBoldChanged: handleSituationChanged();

    onStateChanged: if( debug ) {
                        console.log( state );
                    }
    // Some extra transformations for reverse mode
    transform: Rotation { origin.x: width / 2; origin.y: height / 2; axis { x: 0; y: 1; z: 0 } angle: forward ? 0 : 180 }
    property int reverseX: forward ? 0 : textWidth - width;

    // Deal with either needsMarqueeAtAll or running having changed
    function handleSituationChanged() {
        if( needsMarqueeAtAll ) beginAnimation();
        else endAnimation();
    }

    // Trigger a state change to begin animation
    function beginAnimation() {
        finishedAnimating = false;
        animationDuration = 1 / pixelsPerFrame * 1000 * ( textWidth / 60 );
        animationElapsedTime = animationDuration * ( 1.0 - ( ( text2.x - reverseX ) / ( textWidth + spacing ) ) );
        animationRemainingTime = animationDuration * ( ( text2.x - reverseX ) / ( textWidth + spacing ) );
        if( debug ) {
            console.log( "~~~~state = " + state );
            console.trace();
            console.log( "beginAnimation(), elapsed = " + animationElapsedTime
                        + " remaining = " + animationRemainingTime );
        }

        // We need to begin animating:
        if( running )
            state = ( state === "idle" ? "fullLoop" : "partialLoop" );

        // We need to stop animating:

        // If we were already idle, we're done, keep it that way
        // Simulate the state change *to* idle by calling endAnimation() immediately
        // Fixes an issue where handleSituationChanged() then beginAnimation() is called when this Component finishes instantiating
        else if( state === "idle" ) {
            endAnimation();
        }

        // If text2 is more than halfway through its journey, just finish it
        else if( text2.x - reverseX < ( textWidth + spacing ) / 2 )
            state = "resetLeft";

        // Otherwise, retreat!
        else
            state = "resetRight";

    }

    // Restore the default position and bindings for the text
    // State change to idle implicitly stops any running animation
    function endAnimation() {
        finishedAnimating = true;
        state = "idle";
        text1.x = 0;
        text2.x = Qt.binding( function() {
            return needsMarqueeAtAll ? textWidth + spacing : parent.width + spacing;
        } );
    }

    states: [

        // Pretty much a null state
        State {
            name: "idle";
        },

        // Animate infintely
        State {
            name: "fullLoop";
        },

        // From an arbitrary position, complete what remains of the loop then start the normal loop
        State {
            name: "partialLoop";
        },

        // These two are for when the marquee text loses focus and needs to reset back to its elided position
        // resetLeft is also the initial state
        State {
            name: "resetLeft";
        },
        State {
            name: "resetRight";
        }

    ]

    transitions: [

        // Instantly move text into starting position, animate the text going left, then repeat
        Transition {
            to: "fullLoop";
            SequentialAnimation {
                loops: Animation.Infinite;
                PropertyAction { target: text1; property: "x"; value: 0 + reverseX; }
                PropertyAction { target: text2; property: "x"; value: textWidth + spacing + reverseX; }
                ParallelAnimation {
                    PropertyAnimation { target: text1; duration: animationDuration; property: "x"; to: -textWidth - spacing + reverseX; }
                    PropertyAnimation { target: text2; duration: animationDuration; property: "x"; to: 0 + reverseX; }
                }
            }
        },

        // From an arbitrary position, complete what remains of the loop then start the normal loop
        Transition {
            to: "partialLoop";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: text1; duration: animationRemainingTime < 10 ? 10 : animationRemainingTime; property: "x"; to: -textWidth - spacing + reverseX; }
                    PropertyAnimation { target: text2; duration: animationRemainingTime < 10 ? 10 : animationRemainingTime; property: "x"; to: 0 + reverseX; }
                }
            }
            onRunningChanged: {
                if ( ( state === "partialLoop" ) && ( !running ) )
                    marqueeTextRoot.state = "fullLoop";
            }
        },

        // Exit stage left
        Transition {
            to: "resetLeft";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: text1; easing.type: Easing.OutQuad; duration: animationRemainingTime < 10 ? 10 : animationRemainingTime; property: "x"; to: -textWidth - spacing + reverseX; }
                    PropertyAnimation { target: text2; easing.type: Easing.OutQuad; duration: animationRemainingTime < 10 ? 10 : animationRemainingTime; property: "x"; to: 0 + reverseX; }
                }
                PropertyAction { target: marqueeTextRoot; property: "finishedAnimating"; value: true; }
                PropertyAction { target: text1; property: "x"; value: 0; }
                PropertyAction { target: text2; property: "x"; value: textWidth + spacing; }
            }
            onRunningChanged: {
                if ( ( state === "resetLeft" ) && ( !running ) )
                    marqueeTextRoot.state = "idle";
            }
        },

        // Exit stage right
        Transition {
            to: "resetRight";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: text1; easing.type: Easing.OutQuad; duration: animationElapsedTime < 10 ? 10 : animationElapsedTime; property: "x"; to: 0 + reverseX; }
                    PropertyAnimation { target: text2; easing.type: Easing.OutQuad; duration: animationElapsedTime < 10 ? 10 : animationElapsedTime; property: "x"; to: textWidth + spacing + reverseX; }
                }
                PropertyAction { target: marqueeTextRoot; property: "finishedAnimating"; value: true; }
                PropertyAction { target: text1; property: "x"; value: 0; }
                PropertyAction { target: text2; property: "x"; value: textWidth + spacing; }
            }
            onRunningChanged: {
                if ( ( state === "resetRight" ) && ( !running ) )
                    marqueeTextRoot.state = "idle";
            }
        }

    ]

    // This one's always visible
    Text {
        id: text1;
        width: parent.width;
        x: 0;

        elide: running || !finishedAnimating ? Text.ElideNone : ( forward ? Text.ElideRight : Text.ElideLeft );
        horizontalAlignment: needsMarqueeAtAll ? undefined : parent.horizontalAlignment;

        transform: Rotation { origin.x: width / 2; origin.y: height / 2; axis { x: 0; y: 1; z: 0 } angle: forward ? 0 : 180 }

        text: parent.text;
        color: parent.color;
        font { pixelSize: parent.fontSize; family: fontFamily; bold: parent.bold; }
    }

    // This text is only visible when animating
    // Since its elide property is never changed, its contentWidth never changes either, making it perfect for getting
    // the true width of the text (textWidth) at any given time
    Text {
        id: text2;
        width: parent.width;

        // Stay out of the way unless needed
        x: needsMarqueeAtAll ? textWidth + spacing : parent.width + spacing;

        transform: Rotation { origin.x: width / 2; origin.y: height / 2; axis { x: 0; y: 1; z: 0 } angle: forward ? 0 : 180 }

        text: parent.text;
        color: parent.color;
        font { pixelSize: parent.fontSize; family: fontFamily; bold: parent.bold; }
    }
}

