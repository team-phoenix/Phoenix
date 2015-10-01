import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: gameActionBar;
    height: 50;
    color: Qt.rgba(0,0,0,0.65);
    // radius: height/2;
    // Image {  smooth: true; source: "bg.svg"; anchors.fill: parent; fillMode: Image.TileHorizontally; verticalAlignment: Image.AlignLeft; }

    //  Volumen Icon changer
    property string volumeIcon: {
        if(volumeSlider.value <= 1.0 && volumeSlider.value > 0.5) { volumeIcon: "volume.svg"; }
        if(volumeSlider.value <= 0.5 && volumeSlider.value > 0.0) { volumeIcon: "volumehalf.svg"; }
        if(volumeSlider.value == 0)  { volumeIcon: "volumemute.svg"; }
    }

    // gameActionBar visible only when paused or mouse recently moved and only while not transitioning
    opacity: ( ( ( gameView.coreState === Core.STATEPAUSED ) || ( cursorTimer.running ) )  && ( !layoutStackView.transitioning ) ) ? 1.0 : 0.0;

    Behavior on opacity { PropertyAnimation { duration: 250; } }

    function resetWindowSize() {
        root.minimumWidth = root.defaultMinWidth;
        root.minimumHeight = root.defaultMinHeight;
        if( root.height < root.defaultMinHeight ) { root.height = root.defaultMinHeight; }
        if( root.width < root.defaultMinWidth ) { root.width = root.defaultMinWidth; }
    }

    // Left-Side
    Row {
        id: mediaButtonsRow;
        anchors.fill: parent;
        spacing: 0;
        Rectangle { anchors { top: parent.top; bottom: parent.bottom; } color: "transparent"; width: 10; } // DO NOT remove this - Separator

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
        Row {
            spacing: 0;
            Rectangle {
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 32;
                color: "transparent"
                Button {
                    anchors.centerIn: parent;
                    width: parent.width;
                    iconName: "Volume";
                    iconSource: gameActionBar.volumeIcon;
                    style: ButtonStyle { background: Rectangle { color: "transparent"; } }
                }
                /* Notworking
                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: {
                        volumeSlider.visible = !volumeSlider.visible;
                    }
                    onExited: {
                        volumeSlider.opacity = !volumeSlider.visible;
                    }
                } */
            }

            Slider {
                id: volumeSlider;
                anchors { verticalCenter: parent.verticalCenter; }
                width: 55;
                height: gameActionBar.height;
                minimumValue: 0;
                maximumValue: 1;
                value: maximumValue;
                stepSize: 0.01;
                activeFocusOnPress: true;
                tickmarksEnabled: false;
                onValueChanged: { videoItem.signalSetVolume(value); }

                style: SliderStyle {
                    handle: Item {
                        height: 12;
                        width: 4;
                        Rectangle { id: handleRectangle; anchors.fill: parent; color: "white"; }
                    }

                    groove: Item {
                        width: control.width;
                        height: 2;
                        Rectangle { anchors.fill: parent; color: "#FFFFFF"; opacity: .35; }
                    }
                }
            }
            move: Transition {
                NumberAnimation { properties: "y"; duration: 1000 }
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

    // Right-Side
    Row {
        spacing: 0;
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }

        // Suspend - Minimize
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent"
            width: 32;

            Button {
                anchors.centerIn: parent;
                width: parent.width;
                iconName: "Suspend";
                iconSource: "minimize.svg";
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

        // Shutdown - Close
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent"
            width: 32;

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
        Rectangle { anchors { top: parent.top; bottom: parent.bottom; } color: "transparent"; width: 12; } // DO NOT remove this - Separator
    }
}
