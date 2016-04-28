import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: gameActionBar;
    color: "transparent";

    property int volumeValue: 1.0;

    // Volume Icon changer
    property string volumeIcon: {
        if( volumeSlider.value <= 1.0 && volumeSlider.value > 0.5 ) { volumeIcon: "volume.svg"; }
        if( volumeSlider.value <= 0.5 && volumeSlider.value > 0.0 ) { volumeIcon: "volumehalf.svg"; }
        if( volumeSlider.value == 0 ) { volumeIcon: "volumemute.svg"; }
    }

    Behavior on opacity { PropertyAnimation { duration: 250; } }

    // Background
    Rectangle {
        width: parent.width;
        height: 45;
        color: Qt.rgba( 0, 0, 0, 0.75 );
        radius: 1;

        // Top accent (thin white bar)
        Rectangle {
            width: parent.width - 2;
            height: 1;
            anchors { top: parent.top; topMargin: 1; horizontalCenter: parent.horizontalCenter; }
            opacity: 0.35;
            color: Qt.rgba( 255, 255, 255, 0.35 );
        }

        // Left side
        Row {
            id: mediaButtonsRow;
            anchors.fill: parent;
            anchors.leftMargin: 10;

            // Play - Pause
            Rectangle {
                anchors { top: parent.top; bottom: parent.bottom; }
                color: "transparent";
                width: 32;

                Image {
                    anchors.centerIn: parent;
                    anchors.margins: 10;
                    width: parent.width;
                    sourceSize { height: height; width: width; }
                    source: gameView.running ? qsTr( "pause.svg" ) : qsTr( "play.svg" );
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if( gameView.running ) { coreControl.pause(); }
                        else { coreControl.play(); }
                    }
                }
            }

            // Volume
            Item {
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 28 + 55;
                Row {
                    anchors.fill: parent;

                    Rectangle {
                        id: volIcon;
                        anchors { top: parent.top; bottom: parent.bottom; margins: 10; }
                        width: 28;
                        color: "transparent"

                        Image {
                            anchors.centerIn: parent;
                            width: parent.width;
                            source: gameActionBar.volumeIcon;
                            sourceSize { height: height; width: width; }
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                if ( volumeValue == 1) { volumeValue = 0; }
                                else { volumeValue = 1; }
                            }
                        }
                    }

                    Slider {
                        id: volumeSlider;
                        anchors { verticalCenter: parent.verticalCenter; }
                        width: 55;
                        height: gameActionBar.height;
                        minimumValue: 0;
                        maximumValue: 1;
                        value: volumeValue;
                        stepSize: 0.01;
                        activeFocusOnPress: true;
                        tickmarksEnabled: false;
                        onValueChanged: { coreControl.volume = value; }

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
                    move: Transition { NumberAnimation { properties: "y"; duration: 1000 } }
                }

                // FIXME: Non functional, rootMouseArea and gameActionBarMouseArea eat hover events
                /*MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onEntered: {
                        volumeSlider.visible = !volumeSlider.visible;
                    }
                    onExited: {
                        volumeSlider.opacity = !volumeSlider.visible;
                    }
                }*/
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

                Image {
                    anchors.centerIn: parent;
                    anchors.margins: 10;
                    width: 24;
                    height: 24;
                    sourceSize { height: height; width: width; }
                    source: "blur.svg";
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if( blurEffect.visible ) { blurEffect.visible = false; }
                        else if( !blurEffect.visible ) { blurEffect.visible = true; }
                    }
                }
            }
        }

        // Right side
        Row {
            anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }
            anchors.rightMargin: 12;

            // Toggle TV mode
            Item {
                id: tvmodeToggle;
                anchors.verticalCenter: parent.verticalCenter;
                width: 32;
                height: width;

                Image {
                    id: tvLabel;
                    anchors.fill: parent;
                    anchors.margins: 4;
                    source: "tv.svg"
                    smooth: false;
                    sourceSize { height: height; width: width; }
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        // off -> 16:9
                        if( !videoOutput.television ) {
                            videoOutput.television = true;
                            videoOutput.widescreen = true;
                            tvLabel.source = "tv169.svg";
                            return;
                        }
                        // 16:9 -> off
                        if( videoOutput.television && videoOutput.widescreen ) {
                            videoOutput.television = false;
                            videoOutput.widescreen = false;
                            tvLabel.source = "tv.svg"
                            return;
                        }
                    }

                }
            }

            // Suspend/minimize
            Item {
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 32;

                Image {
                    anchors.centerIn: parent;
                    anchors.margins: 10;
                    width: parent.width;
                    source: "minimize.svg";
                    sourceSize { height: height; width: width; }
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        console.log( "GameActionBar: Minimize (Pause game)" );
                        // These callbacks are necessary as the call to pause() is non-blocking, it just sends out a signal
                        // The callback will execute when it *actually* pauses
                        coreControl.stateChanged.connect( pausedCallback );
                        coreControl.pause();
                    }
                    function pausedCallback( newState ) {
                        // console.log( "pausedCallback(" + newState + ")" );
                        if( newState === Control.PAUSED ) {

                            // This callback is meant to be used until the pause goes through.
                            // Disconnect once it's done
                            coreControl.stateChanged.disconnect( pausedCallback );

                            root.disableMouseClicks();
                            rootMouseArea.hoverEnabled = false;
                            resetCursor();
                            resetWindowSize();
                            layoutStackView.push( mouseDrivenView );

                        }
                    }
                }
            }

            // Fullscreen
            Item {
                id: fullscreenItem;
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 24;

                Shortcut {
                    sequence: StandardKey.FullScreen;
                    onActivated: fullscreenItem.prepareFullscreen();
                }

                Image {
                    anchors.centerIn: parent;
                    height: 18;
                    width: 18;
                    sourceSize { height: height; width: width; }
                    property string screenIcon: {
                        if ( root.visibility === Window.FullScreen ) screenIcon: "window.svg";
                        else if ( root.visibility === Window.Windowed | Window.Maximized ) screenIcon: "fullscreen.svg";
                    }
                    source: screenIcon;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: fullscreenItem.prepareFullscreen();
                    }
                }


                function prepareFullscreen() {
                    // If running, ensure the game is paused while the fullscreen transition happens
                    // Only really applies to OS X
                    if( gameView.running ) {
                        coreControl.stateChanged.connect( stateCallback );
                        coreControl.pause();
                    }

                    // Otherwise, just do the transition
                    else {
                        toggleFullscreen();
                    }
                }

                // Called when the game (on a separate thread) finally pauses
                function stateCallback( newState ) {
                    coreControl.stateChanged.disconnect( stateCallback );
                    toggleFullscreen();
                    coreControl.play();
                }

                function toggleFullscreen() {
                    if ( root.visibility === Window.FullScreen )
                        root.visibility = Window.Windowed;
                    else if ( root.visibility & ( Window.Windowed | Window.Maximized ) )
                        root.visibility = Window.FullScreen;
                }
            }

            // Shutdown/close
            Item {
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 32;

                Image {
                    anchors.centerIn: parent;
                    anchors.margins: 10;
                    width: parent.width;
                    source: "shutdown.svg";
                    sourceSize { height: height; width: width; }
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        console.log( "GameActionBar: Close game" );

                        // Do not transition back to the library until we've *fully* shut down (deleted threads)
                        coreControl.stateChanged.connect( stoppedCallback );

                        coreControl.stop();

                        // Let the user know we're thinking!
                        rootMouseArea.cursorShape = Qt.BusyCursor;
                    }

                    function stoppedCallback( newState ) {
                        console.log( "stoppedCallback(" + newState + ")" );
                        if( newState === Control.STOPPED ) {
                            coreControl.stateChanged.disconnect( stoppedCallback );

                            console.log( "Going to library" );

                            root.resetWindowSize();
                            root.resetTitle();
                            root.disableMouseClicks();
                            rootMouseArea.hoverEnabled = false;
                            layoutStackView.push( mouseDrivenView );
                        }
                    }
                }
            }
        }
    }
}
