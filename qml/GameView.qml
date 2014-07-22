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
    property string gameName: "";
    property string coreName: "";
    property string systemDirectory: "";
    property string saveDirectory: "";
    property bool loadSaveState: false
    property real volumeLevel: 1.0;

    onLoadSaveStateChanged: {
        if (loadSaveState)
            videoItem.loadGameState();
    }

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
        interval: 4000;
        running: true;

        onTriggered: {
            rootMouse.cursorShape = Qt.BlankCursor;
            if (bubbleMenu.visible)
                bubbleMenu.visible = false;
            if (volumeDropDown.visible) {
                volumeDropDown.visible = false;
                volumeBtn.checked = false;
            }

            toolBar.visible = false;
        }

    }

    VideoItem {
        id: videoItem;
        focus: true;
        anchors.fill: parent;
        systemDirectory: root.systemDirectory;
        saveDirectory: root.saveDirectory;
        game: root.gameName;
        libcore: root.coreName;
        run: root.run;
        volume: root.volumeLevel;
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


        Component.onDestruction: {
            saveGameState();
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
        id: volumeDropDown;
        color: "#363535";
        height: 200;
        width: 50;
        anchors {
            right: parent.right;
            bottom: toolBar.top;
            bottomMargin: 25;
        }
        visible: false;

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: mouseTimer.stop();
            onExited: mouseTimer.restart();
        }

        Slider {
            anchors.centerIn: parent;
            orientation: Qt.Vertical;
            height: parent.height * 0.8;
            width: 5;
            stepSize: 0.05;
            minimumValue: 0.0;
            maximumValue: 1.0;
            value: root.volumeLevel;
            onValueChanged: {
                root.volumeLevel = value;
                if (value > 0.8)
                    volumeBtn.backgroundImage = "../assets/volume-high-8x.png";
                else if (0.8 > value && value > 0.0)
                    volumeBtn.backgroundImage = "../assets/volume-low-8x.png";
                else if (value == 0.0)
                    volumeBtn.backgroundImage = "../assets/volume-off-8x.png";

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
                    id: saveBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Save";
                    height: 15;
                    width: 15;
                    onClicked: videoItem.saveGameState();
                }

                Button {
                    id: loadBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Load";
                    height: 20;
                    width: 20;
                    onClicked: videoItem.loadGameState();
                }


                Button {
                    id: volumeBtn;
                    anchors.verticalCenter: parent.verticalCenter;
                    height: 20;
                    width: 20;
                    checkable: true;
                    checked: false;

                    property string backgroundImage: "../assets/volume-high-8x.png";
                    onCheckedChanged: {
                        if (checked) {
                            volumeDropDown.visible = true;
                            volumeDropDown.anchors.rightMargin = 268;
                        }
                        else {
                            volumeDropDown.visible = false;
                        }
                    }

                    style: ButtonStyle {
                        background: Image {
                            source: volumeBtn.backgroundImage;
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
