import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import VideoItem 1.0

ApplicationWindow {
    id: root;
    width: 800;
    height: 600;
    visible: true;
    visibility: "Windowed";
    title: "Phoenix";

    MouseArea {
        id: rootMouse;
        anchors.fill: parent;
        hoverEnabled: true
        onEntered: toolBar.visible = true;
        onMouseXChanged: {
            if (cursorShape !== Qt.ArrowCursor)
                cursorShape = Qt.ArrowCursor;
            if (!toolBar.visible)
                toolBar.visible = true;


            mouseTimer.restart();
        }
        onDoubleClicked: {
            if (visibility == 5)
                visibility = "Windowed";
            else
                visibility = "FullScreen";
        }

    }

    Timer {
        id: mouseTimer;
        interval: 2000;
        running: true;

        onTriggered: {
            rootMouse.cursorShape = Qt.BlankCursor;
            if (bubbleMenu.visible)
                bubbleMenu.visible = false;


            toolBar.visible = false;
        }

    }

    VideoItem {
        id: videoItem;
        focus: true;
        anchors.fill: parent;
        systemDirectory: "C:\\Users\\lee\\Desktop";
        onRunChanged: {
            if (run)
                playBtn.iconImage = "assets/pause.png";
            else
                playBtn.iconImage = "assets/play.png";
        }

        onGamepadScanChanged: {
            if (gamepadScan) {
                //run = false;
                scanGamePad();
                //run = true;
            }
        }

        onWindowVisibilityChanged: visibility = windowVisibility;

        // Eventually have VideoItem not load anything on creation
        // Will run when core and game paths have been entered through
        // qml.

        // Each core could be it's own qml class/type, would allow easy
        // extending through qml and keep it so we wouldn't need to
        // touch c++ side. Just an idea.

        // Also keep last used core loaded on
        // frontend startup. Will hopefully reduce load time of game.

        Component.onCompleted: {

            // libcore must be defined before game is,
            // also they both must reside in Component.onCompleted {}
            libcore = "C:/Users/lee/Desktop/32_cores/bsnes_balanced_libretro.dll";
            game = "C:/Users/lee/Documents/Emulation/SNES/Super Mario All-Stars + Super Mario World (USA).sfc";

            // run must be defined here
            run = true;

        }

        onFpsChanged: {
            fps.text = "FPS: %1".arg(this.fps);
        }

        Text {
            id: fps;
            text: "";
            color: "white";
            font.pointSize: 16;
            style: Text.Outline;
            styleColor: "black";
            anchors.right: parent.right;
            anchors.top: parent.top;
            anchors.rightMargin: 5;
            anchors.topMargin: 5;
        }

    }

    Rectangle {
        id: bubbleMenu;
        visible: false;
        width: 100;
        height: 300;
        color: "#363535";
        radius: 3;
        anchors {
            bottom: toolBar.top;
            right: parent.right;
            rightMargin: 175;
            bottomMargin: 25;
        }

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: mouseTimer.stop();
            onExited: mouseTimer.restart();
        }

        ListView {
            anchors {
                fill: parent;
                topMargin: 20;
            }

            model: ListModel {
                ListElement {title: "Scan";}
                ListElement {title: "";}
                ListElement {title: "";}
                ListElement {title: "";}

            }
            spacing: 20;
            delegate: Row {

                Slider {
                    anchors.verticalCenter: parent.verticalCenter;
                }

                Button {
                    anchors.verticalCenter: parent.verticalCenter;
                    text: title;
                    width: 15;
                    height: 15;
                    onClicked: {
                        videoItem.gamepadScan = true;
                    }
                }
            }
        }

    }

    Rectangle {
        id: toolBar;
        visible: false;
        anchors {
            left: parent.left
            right: parent.right;
            bottom: parent.bottom;
            margins: 20;
        }

        height: 35;
        color: "#1e1e1e";
        radius: 4

        MouseArea {

            anchors.fill: parent;
            hoverEnabled: true
            onEntered: mouseTimer.stop();
            onExited: mouseTimer.restart();
        }
        //border.width: 1
        //border.color: "white"

        Item {
            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
                leftMargin: 6;
            }

            Row {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 200;
                spacing: 25;

                Button {
                    id: quitBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    style: ButtonStyle {
                        background: Image {
                            source: "assets/power.png";
                            sourceSize.width: 20;
                            sourceSize.height: 20;
                        }
                    }
                    onClicked: Qt.quit(0);
                }

                Button {
                    id: playBtn;
                    property string iconImage: "assets/play.png";
                    anchors.verticalCenter: parent.verticalCenter;
                    style: ButtonStyle {
                        background: Image {
                            source: playBtn.iconImage;
                            sourceSize.width: 20;
                            sourceSize.height: 20;
                        }
                    }
                    onClicked:  {
                        if (videoItem.run)
                            videoItem.run = false
                        else
                            videoItem.run = true;
                    }

                }


            }

            Row {
                anchors {
                    right: parent.right;
                    rightMargin: 50;
                    verticalCenter: parent.verticalCenter;
                }
                spacing: 45;

                /*Button {
                    id: audioBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    property string backgroundImage: "assets/volume-mute.png"
                    style: ButtonStyle {
                        background: Image {
                            id: volumeBackground;
                            source: audioBtn.backgroundImage;
                            sourceSize.width: 35;
                            sourceSize.height: 35;
                        }
                    }
                    onClicked: {
                        if (backgroundImage === "assets/volume-mute.png")
                            backgroundImage = "assets/ios7-volume-high.png";
                        else
                            backgroundImage = "assets/volume-mute.png";
                    }

                }*/

                Button {
                    id: qualityBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    style: ButtonStyle {
                        background: Image {
                            source: "assets/gear-a.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }
                }

                Button {
                    id: favoriteBtn;
                    style: ButtonStyle {
                        background: Image {
                            source: "assets/star.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }
                }

                Button {
                    id: gamepadBtn;
                    style: ButtonStyle {
                        background: Image {
                            source: "assets/game-controller-b.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }
                    onClicked: {
                        bubbleMenu.anchors.rightMargin = 100;
                        if (bubbleMenu.visible)
                            bubbleMenu.visible = false;
                        else {
                            bubbleMenu.visible = true;
                        }
                    }
                }

                Button {
                    id: resizeBtn;
                    onClicked: {
                        if (root.visibility === 5)
                            root.visibility = "Windowed";
                        else
                            root.visibility = "FullScreen";
                    }

                    anchors.verticalCenter: parent.verticalCenter;
                    style: ButtonStyle {
                        background: Image {
                            source: "assets/arrow-expand.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }

                }
            }

        }

    }
}
