import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0

Rectangle {
    id: headerBar;
    property string headerColor: "#404040";
    property int fontSize: 13;

    property real sliderValue: 0;
    property bool sliderPressed: false;
    property string viewIcon: "../assets/grid-three-up-8x.png";
    property string folderIcon: "../assets/folder-8x.png";
    property string playIcon: "/assets/GameView/pause.png";
    property bool sliderVisible: true;
    property bool searchBarVisible: true;

    property Timer timer: Timer {
        interval: 4000;
        running: false;

        onTriggered: {
            gameView.gameMouse.cursorShape = Qt.BlankCursor;
            headerBar.height = 0;
            if (volumeDropDown.visible) {
                volumeDropDown.visible = false;
                volumeBtn.checked = false;
            }
        }
    }

    onHeightChanged: {
        if (height == 0)
            visible = false;
        else
            visible = true;
    }

    width: 300;
    height: 50;
    color: headerColor;

    Rectangle {
        id: volumeDropDown;
        color: "#363535";
        height: 200;
        width: 50;
        anchors {
            left: parent.left;
            leftMargin: 125;
            top: headerBar.bottom;
            bottomMargin: 25;
        }
        visible: false;

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: headerBar.timer.stop();
            onExited: headerBar.timer.start();
        }

        Slider {
            anchors.centerIn: parent;
            orientation: Qt.Vertical;
            height: parent.height * 0.8;
            width: 5;
            stepSize: 0.05;
            minimumValue: 0.0;
            maximumValue: 1.0;
            value: gameView.volumeLevel;
            onValueChanged: {
                gameView.volumeLevel = value;
                if (value > 0.8)
                    volumeBtn.backgroundImage = "../assets/volume-high-8x.png";
                else if (0.8 > value && value > 0.0)
                    volumeBtn.backgroundImage = "../assets/volume-low-8x.png";
                else if (value == 0.0)
                    volumeBtn.backgroundImage = "../assets/volume-off-8x.png";

            }
        }
    }

    Row {
        anchors {
            left: parent.left;
            leftMargin: 20;
            verticalCenter: parent.verticalCenter;
        }
        spacing: 10;

        Button {
            id: settingsBtn;
            height: 30;
            width: 30;
            property string backgroundColor: "#000000FF";
            onHoveredChanged: {
                if (hovered) {
                    opacity = 0.7;
                }
                else
                    opacity = 1.0;
            }
            style: ButtonStyle {
                background: Rectangle {
                    color: settingsBtn.backgroundColor;
                }

                label: Image{
                    source: "../assets/cog-6x.png";
                    sourceSize.height: 23;
                    sourceSize.width: 23;
                }

            }

            onClicked:  {
                if (settingsBubble.visible)
                    settingsBubble.visible = false;
                else {
                    settingsBubble.visible = true;
                }
            }
        }

        FileDialog {
            id: folderDialog;
            selectFolder: true;
            title: "Add Folder to Library";
            visible: false;

        }

        Button {
            id: viewBtn;
            height: 30;
            width: 30;
            property string backgroundColor: "#000000FF";
            property string imageSource: headerBar.viewIcon;
            onHoveredChanged: {
                if (hovered) {
                    opacity = 0.7;
                }
                else
                    opacity = 1.0;
            }
            style: ButtonStyle {
                background: Rectangle {
                    color: viewBtn.backgroundColor;
                }

                label: Image{
                    source: viewBtn.imageSource;
                    //opacity: 0.85;
                    sourceSize.height: 25;
                    sourceSize.width: 25;
                }

            }
            onPressedChanged: {
                if (pressed) {
                    if (headerBar.viewIcon === "../assets/GameView/home.png") {
                        gameView.run = false;
                        windowStack.push({item: homeScreen, replace: true})
                    }
                    else if (windowStack.gameStackItemName === "grid") {
                        imageSource = "../assets/list-8x.png";
                        windowStack.currentItem.stackId.push({item: gameTable, replace: true, immediate: true});
                    }
                    else {
                        imageSource = "../assets/grid-three-up-8x.png";
                        windowStack.currentItem.stackId.push({item: gameGrid, replace: true, immediate: true});
                    }
                }

                else
                    backgroundColor = "#000000FF";
            }
        }

        Button {
            id: playBtn;
            visible: gameView.visible;
            anchors.verticalCenter: parent.verticalCenter;

            style: ButtonStyle {
                background: Image {
                    source: headerBar.playIcon;
                    sourceSize.width: 20;
                    sourceSize.height: 20;
                }
            }
            onClicked:  {
                if (gameView.run)
                    gameView.run = false;
                else
                    gameView.run = true;
            }
        }

        Button {
            id: folderBtn;
            property string backgroundColor: "#000000FF";
            visible: !gameView.visible;
            height: 30;
            width: 30;
            onHoveredChanged: {
                if (hovered) {
                    opacity = 0.7;
                }
                else
                    opacity = 1.0;
            }
            style: ButtonStyle {
                background: Rectangle {
                    color: folderBtn.backgroundColor;
                }

                label: Image{
                    source: headerBar.folderIcon;
                    //opacity: 0.85;
                    sourceSize.height: 25;
                    sourceSize.width: 25;
                }

            }
            onPressedChanged: {
                if (pressed) {
                    folderDialog.visible = true;
                }
            }
        }

        Button {
            id: volumeBtn;
            visible: gameView.visible;
            anchors.verticalCenter: parent.verticalCenter;
            height: 20;
            width: 20;
            checkable: true;
            checked: false;

            property string backgroundImage: "../assets/volume-high-8x.png";
            onHoveredChanged: {
                if (hovered) {
                    opacity = 0.7;
                }
                else
                    opacity = 1.0;
            }
            onCheckedChanged: {
                if (checked) {
                    volumeDropDown.visible = true;
                }
                else {
                    volumeDropDown.visible = false;
                }
            }

            style: ButtonStyle {
                background: Image {
                    source: volumeBtn.backgroundImage;
                    sourceSize.width: volumeBtn.width;
                    sourceSize.height: volumeBtn.height;
                }
            }
        }

        Slider {
            id: zoomSlider;
            visible: headerBar.sliderVisible;
            width: 150;
            height: 5;
            anchors {
                verticalCenter: parent.verticalCenter;
            }
            stepSize: 1;
            minimumValue: 1;
            maximumValue: 10;
            value: 5;

            Settings {
                category: "UI";
                property alias zoomFactor: zoomSlider.value;
            }

            onPressedChanged: {
                if (pressed)
                    headerBar.sliderPressed = true;
                else
                    headerBar.sliderPressed = false;
            }

            onValueChanged: {
                var prev = headerBar.sliderValue;
                headerBar.sliderValue = value;

            }

            style: SliderStyle {
                handle: Item {
                    height: 18;
                    width: 18;

                    Rectangle {
                        id: zoomHandle;
                        color: "#f1f1f1";
                        radius: 10;
                        anchors.fill: parent;
                    }


                    DropShadow {
                        source: zoomHandle;
                        anchors.fill: source;
                        horizontalOffset: 1;
                        verticalOffset: 3;
                        radius: 2;
                        samples: 4;
                        color: "#80000000"
                        //transparentBorder: true;
                    }
                }

                groove: Rectangle {
                    implicitHeight: 5;
                    implicitWidth: 200;
                    radius: 2;
                    opacity: 0.8;
                    color: "#1a1a1a";
                }
            }
        }
    }

    Row {
        spacing: 10;
        anchors {
            right: parent.right;
            rightMargin: 25;
            verticalCenter: parent.verticalCenter;
        }

        Button {
            id: saveBtn;
            visible: gameView.visible;
            anchors.verticalCenter: parent.verticalCenter;
            text: "Save";
            height: 15;
            width: 15;
            onClicked: gameView.saveGameState = true;
        }

        Button {
            id: loadBtn;
            visible: gameView.visible;
            anchors.verticalCenter: parent.verticalCenter;
            text: "Load";
            height: 20;
            width: 20;
            onClicked: gameView.loadSaveState = true;
        }

        Button {
            id: favoriteBtn;
            visible: gameView.visible;
            height: 28;
            width: 28;
            anchors.verticalCenter: parent.verticalCenter;
            style: ButtonStyle {
                background: Image {
                    source: "/assets/GameView/star.png";
                    sourceSize.width: favoriteBtn.height;
                    sourceSize.height: favoriteBtn.width;
                }
            }
        }

        Button {
            id: resizeBtn;
            visible: gameView.visible;
            height: 26;
            width: 26;
            anchors.verticalCenter: parent.verticalCenter;
            onClicked: {
                if (root.visibility === 5)
                    root.visibility = "Windowed";
                else
                    root.visibility = "FullScreen";
            }
            style: ButtonStyle {
                background: Image {
                    source: "/assets/GameView/arrow-expand.png";
                    sourceSize.width: resizeBtn.width;
                    sourceSize.height: resizeBtn.height;
                }
            }
        }
    }

    /*Label {
        anchors.centerIn: parent;
        text: "Phoenix";
        color: "#f1f1f1";
        font.bold: true;
        font.pixelSize: headerBar.fontSize;
    }*/

    TextField {
        id: searchBar;
        width: 175;
        placeholderText: "Search";
        visible: headerBar.searchBarVisible;
        font {
            bold: true;
            pixelSize: 14;
        }

        textColor: "#f1f1f1";
        height: 25;

        anchors {
            right: parent.right;
            rightMargin: 20;
            verticalCenter: parent.verticalCenter;
        }

        Timer {
            id: searchTimer;
            interval: 300;
            running: false;
            repeat: false;
            onTriggered: gamelibrary.setFilter(searchBar.text);
        }

        onTextChanged: {
            searchTimer.restart();
        }

        style: TextFieldStyle {
            placeholderTextColor: "#f1f1f1";

            background: Rectangle {
                radius: 2;
                opacity: 0.8;
                color: "#1a1a1a";

            }

        }

        Image {
            id: image;
            focus: true;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
                margins: 5;
            }
            visible: (searchBar.displayText == "") ? false : true;
            source: "../assets/delete-4x.png"
            sourceSize.height: 15;
            sourceSize.width: 15;
            MouseArea {
                anchors.fill: parent;
                onClicked: searchBar.text = "";
            }

        }


    }

}
