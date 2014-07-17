import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import VideoItem 1.0

Item {
    id: root;
    width: 800;
    height: 600;
    visible: true;
    property bool run: false;
    //visibility: "Windowed";
    //title: "Phoenix";

    MouseArea {
        id: rootMouse;
        anchors.fill: parent;
        hoverEnabled: true
        onEntered: toolBar.visible = true;
        onMouseXChanged: {
            if (cursorShape !== Qt.ArrowCursor)
                cursorShape = Qt.ArrowCursor;
            if (!toolBar.visible) {
                toolBar.visible = true;
            }


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
        run: root.run;
        onRunChanged: {
            if (run)
                playBtn.iconImage = "/assets/GameView/pause.png";
            else
                playBtn.iconImage = "/assets/GameView/play.png";
        }


        onSetWindowedChanged: {
            if (visibility != 2)
                visibility = "Windowed";
        }

        // Eventually have VideoItem not load anything on creation
        // Will run when core and game paths have been entered through
        // qml.actual

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

            // run must be defined after libcore and game

        }

    }

    Text {
        id: fpsCounter;
        text: "FPS: " + videoItem.fps;
        color: "#f1f1f1";
        font.pointSize: 16;
        style: Text.Outline;
        styleColor: "black";

         anchors {
            right: parent.right;
            top: parent.top;
            rightMargin: 5;
            topMargin: 5;
        }

    }


    Button {
        id: backButton;
        visible: toolBar.visible;
        anchors {
            left: parent.left;
            top: parent.top;
            leftMargin: 15;
            topMargin: 15;
        }

        style: ButtonStyle {
            background: Image {
                source: "../assets/arrow-left-a.png";
                sourceSize.width: 35;
                sourceSize.height: 35;
            }
        }
        onClicked:  {
            root.run = false;
            windowStack.push({item: homeScreen, replace: true });
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
                    id: playBtn;
                    property string iconImage: "/assets/GameView/play.png";
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
                    property string backgroundImage: "/assets/GameView/volume-mute.png"
                    style: ButtonStyle {
                        background: Image {
                            id: volumeBackground;
                            source: audioBtn.backgroundImage;
                            sourceSize.width: 35;
                            sourceSize.height: 35;
                        }
                    }
                    onClicked: {
                        if (backgroundImage === "/assets/GameView/volume-mute.png")
                            backgroundImage = "/assets/GameView/ios7-volume-high.png";
                        else
                            backgroundImage = "/assets/GameView/volume-mute.png";
                    }

                }*/

                Button {
                    id: qualityBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    style: ButtonStyle {
                        background: Image {
                            source: "/assets/GameView/gear-a.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }
                }

                Button {
                    id: favoriteBtn;
                    style: ButtonStyle {
                        background: Image {
                            source: "/assets/GameView/star.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }
                }

                Button {
                    id: gamepadBtn;
                    style: ButtonStyle {
                        background: Image {
                            source: "/assets/GameView/game-controller-b.png";
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
                            source: "/assets/GameView/arrow-expand.png";
                            sourceSize.width: 25;
                            sourceSize.height: 25;
                        }
                    }

                }
            }

        }

    }
}
