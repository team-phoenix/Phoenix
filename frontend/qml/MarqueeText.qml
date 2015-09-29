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

    state: "idle";

    property string text: "Error!";
    property int fontSize: PhxTheme.common.baseFontSize;
    property color color: "white";
    property int spacing: 50;
    property bool running: false;
    property bool finishedAnimating: true;

    property double pixelsPerFrame: 2;
    property double animationDuration: 1 / pixelsPerFrame * 1000 * ( textWidth / 60 );
    property double animationElapsedTime: 0;
    property double animationRemainingTime: 0;

    property double textWidth: text2.contentWidth;
    property bool needsMarqueeAtAll: textWidth > width;

    // Handle the changing width of this Item affecting the need to have the marquee text scroll at all
    onNeedsMarqueeAtAllChanged: {
        if( needsMarqueeAtAll ) {
            if( running ) {
                animationRemainingTime = animationDuration * ( ( text2.x - spacing ) / textWidth );
                if( state === "idle" ) state = "fullLoop";
                else if( state === "resetLeft" || state === "resetRight" ) state = "partialLoop";
            }
        }
        else {
            state = "idle";
            text1.x = 0;
            text2.x = Qt.binding( function() {
                return parent.width + spacing;
            } );
        }
    }

    // Handle the situation when the marquee text loses focus and needs to reset back to its elided position
    // ...or gains focus and needs to have its animation begin
    onRunningChanged: {
        if( needsMarqueeAtAll ) {
            finishedAnimating = false;
            if( running ) {
                animationRemainingTime = animationDuration * ( ( text2.x - spacing ) / textWidth );
                state = ( state === "idle" ? "fullLoop" : "partialLoop" );
            }
            else {
                // If text2 is more than halfway through its journey, just finish it
                if( text2.x < textWidth / 2 ) {
                    animationRemainingTime = animationDuration * ( ( text2.x - spacing ) / textWidth );
                    state = "resetLeft";
                }

                // Otherwise, retreat!
                else {
                    animationElapsedTime = animationDuration * ( 1.0 - ( ( text2.x - spacing ) / textWidth ) );
                    state = "resetRight";
                }
            }
        }
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
                PropertyAction { target: text1; property: "x"; value: 0; }
                PropertyAction { target: text2; property: "x"; value: textWidth + spacing; }
                ParallelAnimation {
                    PropertyAnimation { target: text1; duration: animationDuration; property: "x"; to: -textWidth - spacing; }
                    PropertyAnimation { target: text2; duration: animationDuration; property: "x"; to: 0; }
                }
            }
        },

        // From an arbitrary position, complete what remains of the loop then start the normal loop
        Transition {
            to: "partialLoop";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: text1; duration: animationRemainingTime < 0 ? 0 : animationRemainingTime; property: "x"; to: -textWidth - spacing; }
                    PropertyAnimation { target: text2; duration: animationRemainingTime < 0 ? 0 : animationRemainingTime; property: "x"; to: 0; }
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
                    PropertyAnimation { target: text1; duration: animationRemainingTime < 0 ? 0 : animationRemainingTime; property: "x"; to: -textWidth - spacing; }
                    PropertyAnimation { target: text2; duration: animationRemainingTime < 0 ? 0 : animationRemainingTime; property: "x"; to: 0; }
                }
                PropertyAction { target: text1; property: "x"; value: 0; }
                PropertyAction { target: text2; property: "x"; value: textWidth + spacing; }
                PropertyAction { target: marqueeTextRoot; property: "finishedAnimating"; value: true; }
            }
        },

        // Exit stage right
        Transition {
            to: "resetRight";
            SequentialAnimation {
                ParallelAnimation {
                    PropertyAnimation { target: text1; duration: animationElapsedTime < 0 ? 0 : animationElapsedTime; property: "x"; to: 0; }
                    PropertyAnimation { target: text2; duration: animationElapsedTime < 0 ? 0 : animationElapsedTime; property: "x"; to: textWidth + spacing; }
                }
                PropertyAction { target: marqueeTextRoot; property: "finishedAnimating"; value: true; }
            }
        }

    ]

    // This one's always visible
    Label {
        id: text1;
        width: parent.width;

        elide: running || !finishedAnimating ? Text.ElideNone : Text.ElideRight;
        horizontalAlignment: needsMarqueeAtAll ? undefined : Text.AlignHCenter;

        text: parent.text;
        color: parent.color;
        font { pixelSize: parent.fontSize; }
    }

    // This text is only visible when animating
    // Since its elide property is never changed, its contentWidth never changes either, making it perfect for getting
    // the true width of the text (textWidth) at any given time
    Label {
        id: text2;
        width: parent.width;

        // Stay out of the way unless needed
        x: needsMarqueeAtAll ? textWidth + spacing : parent.width;

        text: parent.text;
        color: parent.color;
        font { pixelSize: parent.fontSize; }
    }
}

