import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

Rectangle {
    height: 55;
    color: Qt.rgba(0,0,0,0.75);

    // actionBar visible only when paused or mouse recently moved and only while not transitioning
    opacity: ( ( ( gameView.coreState === Core.STATEPAUSED ) || ( cursorTimer.running ) )  && ( !layoutStackView.transitioning ) ) ? 1.0 : 0.0;

    Behavior on opacity { PropertyAnimation { duration: 250; } }

    function resetWindowSize() {
        root.minimumWidth = root.defaultMinWidth;
        root.minimumHeight = root.defaultMinHeight;
        if( root.height < root.defaultMinHeight ) { root.height = root.defaultMinHeight; }
        if( root.width < root.defaultMinWidth ) { root.width = root.defaultMinWidth; }
    }

    Row {
        id: mediaButtonsRow;
        anchors.fill: parent;
        spacing: 0;
        Rectangle { anchors { top: parent.top; bottom: parent.bottom; } color: "transparent"; width: 13; } // DO NOT remove this

        // Play - Pause
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent";
            width: 40;

            Button {
                anchors.centerIn: parent;
                width: parent.width;
                iconName: videoItem.running ? qsTr( "Pause" ) : qsTr( "Play" );
                iconSource: videoItem.running ? qsTr( "pause.svg" ) : qsTr( "play.svg" );
                style: ButtonStyle { background: Rectangle { color: "transparent"; } }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if ( videoItem.running ) {
                        videoItem.slotPause();
                    } else {
                        videoItem.slotResume();
                    }
                }
            }
        }
        // Suspend
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent"
            width: 40;

            Button {
                anchors.centerIn: parent;
                width: parent.width;
                iconName: "Suspend";
                iconSource: "suspend.svg";
                style: ButtonStyle { background: Rectangle { color: "transparent"; } }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    videoItem.slotPause();
                    root.disableMouseClicks();
                    rootMouseArea.hoverEnabled = false;
                    resetCursor();
                    resetWindowSize();
                    layoutStackView.push( mouseDrivenView );
                }
            }
        }

        /*/ Settings
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent"
            width: 40;

            Button {
                anchors.centerIn: parent;
                width: parent.width;
                iconName: "Settings";
                iconSource: "settings.svg";
                style: ButtonStyle { background: Rectangle { color: "transparent"; } }
            }
        } */

        // Blur
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent"
            width: 40;

            Label {
                anchors.centerIn: parent;
                color: PhxTheme.normalFontColor;
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

    }

    // Shutdown - Close
    Rectangle {
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }
        color: "transparent"
        width: height;

        Button {
            anchors.centerIn: parent;
            width: parent.width;
            iconName: "Shutdown";
            iconSource: "shutdown.svg";
            style: ButtonStyle { background: Rectangle { color: "transparent"; } }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                videoItem.slotStop();
                root.disableMouseClicks();
                rootMouseArea.hoverEnabled = false;
                resetCursor();
                resetWindowSize();
                layoutStackView.push( mouseDrivenView );
            }
        }
    }
}
