import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0

Rectangle {
    height: 65;
    // actionBar visible only when paused or mouse recently moved and only while not transitioning
    opacity: ( ( ( gameView.coreState === Core.STATEPAUSED ) || ( cursorTimer.running ) )
                && ( !layoutStackView.transitioning ) ) ? .35 : 0.0;

    Behavior on opacity {
        PropertyAnimation { duration: 250; }
    }


    Row {
        id: mediaButtonsRow;
        anchors {
            fill: parent;
        }

        spacing: 0;

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: height;


            Rectangle {
                id: topBorder;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "white";
                opacity: 0.5;
            }

            Rectangle {
                id: bottomBorder;
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "white";
                opacity: 0.05;

            }

            Rectangle {
                id: leftBorder;
                anchors {
                    top: topBorder.bottom;
                    left: parent.left;
                    bottom: bottomBorder.top;
                }
                width: 1;
                color: "white";
                opacity: 0.1;
            }

            Rectangle {
                id: rightBorder;
                anchors {
                    top: topBorder.bottom;
                    right: parent.right;
                    bottom: bottomBorder.top;
                }
                width: 1;
                color: "white";
                opacity: 0.1;
            }

            Label {
                anchors {
                    centerIn: parent;
                }
                text: videoItem.running ? qsTr( "Pause" ) : qsTr( "Play" );
                color: "black";
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if ( videoItem.running ) {
                        videoItem.pause();
                    } else {
                        videoItem.resume();
                    }
                }
            }
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            width: 1;
            color: "black";
            opacity: 0.2;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            color: "#00000000"
            width: height;

            Label {
                anchors.centerIn: parent;
                color: "black";
                text: qsTr( "Blur" );
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if( blurEffect.visible ) {
                        blurEffect.visible = false;
                    }
                    else if( !blurEffect.visible ) {
                        blurEffect.visible = true;
                    }
                }
            }
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            color: "#00000000"
            radius: 0
            width: height;

            Label {
                anchors.centerIn: parent;
                color: "black";
                text: qsTr( "ShutDown" );
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    videoItem.stop();
                    root.disableMouseClicks();
                    rootMouseArea.hoverEnabled = false;
                    resetCursor();
                    layoutStackView.push( mouseDrivenView );
                }
            }
        }
    }

    Rectangle {
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            right: parent.right;
        }

        color: "#00000000"
        width: height;

        Label {
            anchors.centerIn: parent;
            color: "black";
            text: qsTr( "Suspend" );
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                videoItem.pause();
                root.disableMouseClicks();
                rootMouseArea.hoverEnabled = false;
                resetCursor();
                layoutStackView.push( mouseDrivenView );
            }
        }
    }
}


