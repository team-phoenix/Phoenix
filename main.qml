import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import GLWindow 1.0

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

    GLWindow {
        id: glWindow;
        run: false; // default must be true
        focus: true;
        anchors.fill: parent;
        onRunChanged: {
            if (run)
                playBtn.iconImage = "assets/pause.png";
            else
                playBtn.iconImage = "assets/play.png";
        }



        onWindowVisibilityChanged: visibility = windowVisibility;

        // Eventually have GLWindow not load anything on creation
        // Will run when core and game paths have been entered through
        // qml.

        // Each core could be it's own qml class/type, would allow easy
        // extending through qml and keep it so we wouldn't need to
        // touch c++ side. Just an idea.

        // Also keep last used core loaded on
        // frontend startup. Will hopefully reduce load time of game.

        Component.onCompleted: {
            libcore = "C:/Users/robert/Desktop/leesstuff/snes9x_libretro.dll";
            game =  "C:/Users/robert/Desktop/leesstuff/past.sfc";
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
            onEntered: toolBar.visible = true;
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
                        if (glWindow.run) {
                            glWindow.run = false
                        }
                        else {
                            glWindow.run = true;
                        }
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
                    onClicked: {
                        if (bubbleMenu.visible)
                            bubbleMenu.visible = false;
                        else
                            bubbleMenu.visible = true;
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
