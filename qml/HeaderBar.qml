import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
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
    property real volumeLevel: volumeSlider.value;
    property string systemText: "";
    property string previousViewIcon: "";

    property Timer timer: Timer {
        interval: 4000;
        running: false;

        onTriggered: {
            windowStack.currentItem.gameMouse.cursorShape = Qt.BlankCursor;
            headerBar.height = 0;
            if (volumeDropDown.visible) {
                volumeDropDown.visible = false;
                volumeBtn.checked = false;
            }
        }
    }

    gradient: Gradient {
        GradientStop {position: 0.0; color: "#2f2f2f";}
        GradientStop {position: 1.0; color: "#1b1b1b";}
    }

    Column {
        id: topBord;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
            }
            height: 1;
            color: root.borderColor;
        }

        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                rightMargin: 2;
            }
            height: 1;
            color: "#4f4f4f";
        }
    }

    Row {
        id: leftBord;
        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
            topMargin: 2;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                bottomMargin: -1;
                topMargin: -2;
            }
            width: 1
            color: root.borderColor;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#292929";
        }
    }

    Row {
        id: rightBord;
        anchors {
            right: parent.right;
            top: topBord.bottom;
            bottom: parent.bottom;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#292929";
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                topMargin: -1;
                bottomMargin: -1;
            }
            width: 1;
            color: root.borderColor;
        }
    }

    Column {
        id: bottomBord;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        Rectangle {
            id: bottomBorder1;
            anchors {
                left: parent.left;
                right: parent.right;
                leftMargin: 1;
                rightMargin: 1;
            }
            height: 1;
            color: "#292929";
        }

        Rectangle {
            id: bottomBorder2;
            anchors {
                left: parent.left;
                right: parent.right;
            }
            height: 1;
            color: "#0b0b0b";
        }
    }

    onHeightChanged: {
        if (height == 0) {
            visible = false;
        }
        else {
            visible = true;
        }
    }

    width: 300;
    height: 50;
    //color: headerColor;

    Rectangle {
        visible: volumeDropDown.visible;
        z: volumeDropDown.z + 1;
        onVisibleChanged: {
            if (visible)
                headerBar.timer.stop();
            else {
                if (windowStack.currentItem !== null) {
                    if (root.gameShowing)
                        headerBar.timer.start();
                }
            }
        }
        anchors {
            bottom: volumeDropDown.top;
            horizontalCenter: volumeDropDown.horizontalCenter;
            bottomMargin: -8;
        }
        height: 15;
        width: 15;
        color: "#2c2c2c";

        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: 1;
            color: "#3b3b3b";
        }
        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#3b3b3b";
        }

        rotation: 45;
    }

    Rectangle {
        id: volumeDropDown;
        gradient: Gradient {
            GradientStop {position: 0.0; color: "#2e2e2e";}
            GradientStop {position: 1.0; color: "#1b1b1b";}
        }

        height: 200;
        width: 50;
        radius: 5;
        anchors {
            top: headerBar.bottom;
            topMargin: 0;
            horizontalCenter: leftButtonRow.horizontalCenter;
            horizontalCenterOffset: 32;
        }
        border {
            width: 1;
            color: "#333333";
        }

        visible: false;

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: headerBar.timer.stop();
            onExited: headerBar.timer.start();
        }

        Slider {
            id: volumeSlider;
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
            style: SliderStyle {
                id: sliderStyle;

                handle: Item {
                    height: 18;
                    width: 18;

                    Rectangle {
                        id: zoomHandle;
                        gradient: Gradient {
                            GradientStop{ position: 0.0; color: "#f1f1f1";}
                            GradientStop{ position: 0.5; color: "#f1f2f1";}
                            GradientStop{ position: 0.7; color: "#e2e2e2";}
                            GradientStop{ position: 1.0; color: "#cbcacb";}

                        }

                        radius: 10;
                        anchors.fill: parent;
                        smooth: true;
                    }
                }

                groove: Rectangle {
                    height: 6;
                    width: zoomSlider.width;
                    radius: 6;
                    color: "#1a1a1a";

                    Rectangle {
                        color: "#f1f1f1";
                        anchors {
                            left: parent.left;
                            top: parent.top;
                            bottom: parent.bottom;
                        }
                        width: 15 * (volumeSlider.value * 10);
                        // 14 seems to be the magic number.
                        // It's a quick and dirty way to make
                        // the effect work.
                    }

                    Rectangle {
                        // topBorder;
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                        }
                        radius: 3;
                        height: 1;
                        color: "#1f1f1f";
                    }

                    Rectangle {
                        // leftBorder;
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                            left: parent.left;
                        }
                        radius: 3;
                        width: 1;
                        color: "#141414";
                    }

                    Rectangle {
                        // rightBorder;
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                            right: parent.right;
                        }
                        radius: 3;
                        width: 1;
                        color: "#141414";
                    }

                    Rectangle {
                        // bottomBorder;
                        radius: 3;

                        anchors {
                            bottom: parent.bottom;
                            left: parent.left;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#3b3b3b";
                    }
                }
            }
        }
    }

    Row {
        id: leftButtonRow;
        anchors {
            left: parent.left;
            leftMargin: 20;
            verticalCenter: parent.verticalCenter;
        }
        spacing: 15;

        Image {
            id: settingsBtn;
            source: !root.gameShowing ? "../assets/cog-6x.png" : "../assets/GameView/home.png";
            visible: true;
            sourceSize {
                height: 20;
                width: 20;

            }
            anchors.verticalCenter: parent.verticalCenter;
            property string backgroundColor: "#000000FF";

            MouseArea {
                anchors.fill: parent;

                onClicked:  {
                    if (root.itemInView === "game") {
                        windowStack.currentItem.run = false;
                        volumeDropDown.visible = false;
                        windowStack.push({item: homeScreen, replace: true})
                        headerBar.userText = "Phoenix";
                    }
                    else {
                        if (settingsDropDown.visible)
                            settingsDropDown.visible = false;
                        else {
                            settingsDropDown.visible = true;
                        }
                    }
                }
            }
        }

        PhoenixViewButtons {
            height: 32;
            width: height * 2;
            visible: !root.gameShowing;
            anchors.verticalCenter: parent.verticalCenter;
        }

        PhoenixNormalButton {
            id: playBtn;
            visible: root.gameShowing;
            anchors.verticalCenter: parent.verticalCenter;
            height: 30;
            width: 30;

            Image {
                anchors.centerIn: parent;
                source: headerBar.playIcon;
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.checked ? 0.5 : 1.0;
            }

            onPressedChanged:  {
                if (pressed) {
                    if (windowStack.currentItem.run)
                        windowStack.currentItem.run = false;
                    else
                        windowStack.currentItem.run = true;
                }
            }
        }

        PhoenixNormalButton {
            id: volumeBtn;
            visible: root.gameShowing;
            anchors.verticalCenter: parent.verticalCenter;
            height: 30;
            width: 30;
            checkable: true;
            checked: false;

            property string backgroundImage: "../assets/volume-high-8x.png";

            Image {
                anchors.centerIn: parent;
                source: parent.backgroundImage;
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.checked ? 0.5 : 1.0;
            }

            onCheckedChanged: {
                if (checked) {
                    volumeDropDown.visible = true;
                }
                else {
                    volumeDropDown.visible = false;
                }
            }
        }

        Row {
            anchors {
                verticalCenter: parent.verticalCenter;
            }
            visible: !root.gameShowing && !root.clear;

            spacing: 12;

            Image {
                source: "../assets/small-icon.png";
                height: 6;
                width: 6;
                sourceSize {
                    height: 6;
                    width: 6;
                }
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
            }

            Slider {
                id: zoomSlider;
                width: 120;
                height: 25;

                stepSize: 0.5;
                minimumValue: 1.5;
                maximumValue: 5.0;
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
                    id: sliderStyle;

                    handle: Item {
                        height: 24;
                        width: 24;

                        Rectangle {
                            id: zoomHandle;
                            gradient: Gradient {
                                GradientStop{ position: 0.0; color: "#343434";}
                                GradientStop{ position: 0.5; color: "#343433";}
                                GradientStop{ position: 0.7; color: "#2c2d2c";}
                                GradientStop{ position: 1.0; color: "#242424";}

                            }

                            radius: width * 0.5;
                            anchors.centerIn: parent;
                            height: 17;
                            width: 17;

                            CustomBorder {
                                radius: parent.width / 2;
                                gradient: Gradient {
                                    GradientStop{ position: 0.0; color: "#595959";}
                                    GradientStop{ position: 1.0; color: "black";}
                                }
                            }

                            Rectangle {
                                radius: width / 2;
                                anchors {
                                    centerIn: parent;
                                }
                                width: parent.width * 0.36;
                                height: parent.height * 0.36;

                                CustomBorder {
                                    radius: parent.width / 2;
                                    gradient: Gradient {
                                        GradientStop{ position: 0.0; color: "black";}
                                        GradientStop{ position: 1.0; color: "#595959";}
                                    }

                                }

                                gradient: Gradient {
                                    GradientStop{ position: 0.0; color: "#070707";}
                                    GradientStop{ position: 1.0; color: "#222221";}
                                }
                            }

                           //CustomBorder {
                           //    color: "black"
                            //}

                        }
                        /*DropShadow {
                            source: zoomHandle;
                            anchors.fill: source;
                            horizontalOffset: 0;
                            verticalOffset: 1;
                            color: "black";
                            opacity: 1.0;
                            radius: 2;
                            samples: radius * 2;

                        }*/
                    }

                    groove: Rectangle {
                        height: 5;
                        width: zoomSlider.width;
                        radius: 3;
                        opacity: 0.8;
                        color: "#1a1a1a";
                        border {
                            width: 1;
                            color: "black";
                        }

                        /*Rectangle {
                            color: "#f1f1f1";
                            radius: parent.radius;
                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                                left: parent.left;
                                topMargin: 1;
                                bottomMargin: 1;
                                leftMargin: 1;
                            }
                            width: (zoomSlider.value >= 3.0) ? 10  * (zoomSlider.value * 2.0) : 6  * (zoomSlider.value * 2.0);
                        }*/

                        CustomBorder {
                            gradient: Gradient {
                                GradientStop {position: 0.0; color: "#1a1a1a";}
                                GradientStop {position: 0.95; color: "#4a4a4a";}
                            }
                        }

                    }
                }
            }
            Image {
                source: "../assets/big-icon.png";
                height: 11;
                width: 11;
                sourceSize {
                    height: 11;
                    width: 11;
                }
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
            }
        }
    }

    Item {
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            horizontalCenter: parent.horizontalCenter;
        }
        width: 275;

        Row {
            id: leftSeparator;
            anchors {
                top: parent.top;
                topMargin: 2;
                bottom: parent.bottom;
                bottomMargin: 2;
                left: parent.left;
            }

            Rectangle {
                width: 1;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }

                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#4f4f4f";}
                    GradientStop {position: 1.0; color: "#1c1c1c";}
                }
            }

            Rectangle {
                width: 1;
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#070707";}
                    GradientStop {position: 1.0; color: "#1d1d1d";}
                }

                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
            }

        }

        Rectangle {
            anchors {
                top: parent.top;
                topMargin: 2;
                bottom: parent.bottom;
                bottomMargin: 2;
                left: leftSeparator.right;
                right: rightSeparator.left;
            }
            color: "transparent";

            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: parent.color = "#1d1d1d";
                onExited: parent.color = "transparent";
            }

            Column {
                anchors {
                    centerIn: parent;
                }

                spacing: 4;

                Text {
                    id: userAreaText;
                    text: root.lastGameName;
                    renderType: Text.QtRendering;
                    font {
                        family: "Sans";
                        pixelSize: text !== "Phoenix" ? 12 : 14;
                        bold: true;
                    }
                    anchors.horizontalCenter: parent.horizontalCenter;
                    color: "#d9d9d9";
                }
                Text {
                    id: systemText;
                    text: root.lastSystemName;
                    renderType: Text.QtRendering;
                    elide: Text.ElideRight;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    font {
                        family: "Sans";
                        pixelSize: 11;
                    }
                    visible: text !== "Phoenix";
                    color: "#d9d9d9";
                }
            }
        }

        Row {
            id: rightSeparator;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                right: parent.right;
                topMargin: 2;
                bottomMargin: 2;
            }
            Rectangle {
                width: 1;
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#070707";}
                    GradientStop {position: 1.0; color: "#1d1d1d";}
                }

                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
            }
            Rectangle {
                width: 1;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#4f4f4f";}
                    GradientStop {position: 1.0; color: "#1c1c1c";}
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

        PhoenixNormalButton {
            id: saveBtn;
            visible: root.gameShowing;
            anchors.verticalCenter: parent.verticalCenter;
            height: 30;
            width: 30;
            onClicked: windowStack.currentItem.saveGameState = true;

            Image {
                anchors.centerIn: parent;
                source: "../assets/GameView/save-state.png";
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.pressed ? 0.5 : 1.0;
            }
        }

        PhoenixNormalButton {
            id: loadBtn;
            visible: root.gameShowing;
            anchors.verticalCenter: parent.verticalCenter;
            height: 30;
            width: 30;
            onClicked: windowStack.currentItem.loadSaveState = true;

            Image {
                anchors.centerIn: parent;
                source: "../assets/GameView/load-state.png";
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.pressed ? 0.5 : 1.0;
            }
        }

        PhoenixNormalButton {
            id: favoriteBtn;
            visible: root.gameShowing;
            height: 30;
            width: 30;
            anchors.verticalCenter: parent.verticalCenter;
            Image {
                anchors.centerIn: parent;
                source: "/assets/GameView/favorite-empty.png";
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.pressed ? 0.5 : 1.0;
            }
        }

        PhoenixNormalButton {
            id: resizeBtn;
            visible: root.gameShowing;
            height: 30;
            width: 30;
            anchors.verticalCenter: parent.verticalCenter;
            onClicked: {
                root.swapScreenSize();
            }

            Image {
                anchors.centerIn: parent;
                source: "/assets/GameView/fullscreen.png";
                sourceSize.height: settingsBtn.height * 0.6;
                sourceSize.width: settingsBtn.width * 0.6;
                opacity: parent.pressed ? 0.5 : 1.0;
            }
        }
    }

    PhoenixTextField {
        id: searchBar;
        placeholderText: "Search";
        placeholderTextColor: textColor;
        visible: !root.gameShowing;
        radius: 3;
        font {
            pixelSize: 12;
            family: "Sans";
        }
        textColor: "#f1f1f1";
        height: 25;
        width: 175;
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
            onTriggered: phoenixLibrary.model().setFilter("title LIKE ?", ['%'+searchBar.text+'%']);
        }

        onTextChanged: {
            searchTimer.restart();
        }

        Image {
            id: image;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
                margins: 5;
            }
            visible: (searchBar.displayText === "") ? false : true;
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
