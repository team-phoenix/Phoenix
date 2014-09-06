import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.0
import Qt.labs.settings 1.0
import phoenix.window 1.0
import phoenix.library 1.0
import QtQuick.Window 2.0

PhoenixWindow {
    id: root;
    width: 640
    height: 480
    minimumHeight: 480;
    minimumWidth: 640;
    swapInterval: 0;
    flags: flagValue;
    frameless: (flagValue === "FramelessWindowHint");
    title: "Phoenix";

    property bool clear: false;
    property string accentColor:"#e8433f";
    property int lastWindowStyle: Window.Windowed;
    property string borderColor: "#0b0b0b";
    property string flagValue: "";


    function swapScreenSize(){
        if (root.visibility == Window.FullScreen)
            root.visibility = lastWindowStyle;
        else {
            lastWindowStyle = root.visibility;
            root.visibility = Window.FullScreen;
        }
    }

    Rectangle {
        id: leftBorder;
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
        }
        width: 1;
        color: borderColor;
    }

    Rectangle {
        id: rightBorder;
        anchors {
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
        }
        width: 1;
        color: borderColor;
    }

    Rectangle {
        id: bottomBorder;
        anchors {
            bottom: bottom.top;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: borderColor;
    }

    onWidthChanged: {
        settingsDropDown.state = "retracted";
    }

    PhoenixLibrary {
        id: phoenixLibrary;
    }

    MouseArea {
        anchors.fill: parent;
        enabled: settingsDropDown.visible;
        onClicked: settingsDropDown.visible = false;
    }

    Component {
        id: gameGrid;
        GameGrid {
            property string itemName: "grid";
            color: "#262626";
            zoomFactor: headerBar.sliderValue;
            zoomSliderPressed: headerBar.sliderPressed;

        }
    }

    Component {
        id: gameTable;
        GameTable {
            itemName: "table";
            highlightColor: "#4a4a4a";
            textColor: "#f1f1f1";
            headerColor: "#262626";
        }
    }

    Settings {
        id: settings;
        category: "UI";
        //property alias windowX: root.x;
       // property alias windowY: root.y;
        //property alias windowWidth: root.width;
        //property alias windowHeight: root.height;
        property alias volumeLevel: gameView.volumeLevel;
    }

    HeaderBar {
        id: headerBar;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        Behavior on height {
            NumberAnimation {
                duration: 150;
            }
        }

        height: 55;
        //color: "#3b3b3b";
        fontSize: 14;
    }

    SettingsDropDown {
        id: settingsDropDown;
        z: headerBar.z + 1;
        visible: false;
        opacity: visible ? 1.0 : 0.0;

        Behavior on opacity {
            NumberAnimation {
                easing {
                    type: Easing.OutQuad;
                }
                duration: 200;
            }
        }

        anchors {
            top: headerBar.bottom;
            topMargin: 5;
            left: parent.left;
            leftMargin: 10;
        }
        height: 200;

        stackBackgroundColor: "#f4f4f4";
        contentColor: "#f4f4f4";
        textColor: "#f1f1f1";

        Rectangle {
            visible: parent.visible;
            opacity: parent.visible ? 1.0 : 0.0;
            height: 20;
            width: 20;
            rotation: 45;
            color: "#2f2f2f";
            z: settingsDropDown.z + 1;

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    leftMargin: 1;
                    right: parent.right;
                }
                height: 1;
                color: "#0b0b0b";
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "#0b0b0b";
            }

            Behavior on opacity {
                NumberAnimation {
                    easing {
                        type: Easing.OutQuad;
                    }
                    duration: 200;
                }
            }

            anchors {
                left: parent.left;
                leftMargin: 14;
                verticalCenter: settingsDropDown.top;
            }

        }

    }

    DropShadow {
        source: settingsDropDown;
        anchors.fill: source;
        horizontalOffset: 1;
        verticalOffset: 2;
        visible: source.visible;
        radius: 4;
        samples: radius * 2;
        color: "#80000000";
        transparentBorder: true;
    }

    StackView {
        id: windowStack;
        z: headerBar.z - 1;

        height: headerBar.visible ? (parent.height - headerBar.height) : (parent.height);
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        initialItem: homeScreen;

        property string gameStackItemName: {
            if (currentItem != null && typeof currentItem.stackName !== "undefined") {
                console.log(currentItem.stackName);
                return currentItem.stackName;
            }
            else {
                return "";
            }
        }

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.x = 0;
            }
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem;
                    property: "x";
                    from: -enterItem.width;
                    to: 0;
                    duration: 600;
                }
                PropertyAnimation {
                    target: exitItem;
                    property: "x";
                    from: 0;
                    to: -exitItem.width;
                    duration: 600;
                }
            }
       }
    }

    GameView {
        id: gameView;
        visible: false;
        systemDirectory: "C:/Users/lee/Desktop";
        saveDirectory:  systemDirectory;
    }

    Component {
        id: homeScreen;

        Item {
            property string stackName: "homescreen";
            property StackView stackId: gameStack;


            ConsoleBar {
                id: consoleBar;
                z: headerBar.z - 1;
                color: "#262626";
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 200;
            }

            StackView {
                id: gameStack;
                z: windowStack.z;

                onCurrentItemChanged: {
                    if (currentItem)
                    parent.stackName = currentItem.itemName;
                }

                initialItem: {
                    if (root.clear === true)
                        return emptyScreen;

                    return gameGrid;
                }

                anchors {
                    left: consoleBar.right;
                    right: parent.right;
                    top: parent.top;
                    bottom: parent.bottom;
                }
            }

            Component {
                id: emptyScreen;

                Rectangle {
                    property string itemName: "empty";
                    color: "#1d1e1e";

                    Column {
                        anchors.centerIn: parent;
                        spacing: 30;

                        Column {
                            //anchors.horizontalCenter: parent.horizontalCenter;
                            spacing: 2;
                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter;
                                text: "Get Some Games"
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 26;
                                }
                                horizontalAlignment: Text.AlignHCenter;
                            }

                            Label {

                                text: "Phoenix can't seem to find your games."
                                color: "gray";
                                font {
                                    family: "Sans";
                                    pixelSize: 16;
                                }
                                horizontalAlignment: Text.AlignHCenter;
                            }

                        }


                        FileDialog {
                            id: pathDialog;
                            selectFolder: true;
                            title: "Add Game Folder";

                        }

                        Button {
                            id: importGamesBtn;
                            onClicked: pathDialog.open();
                            property string backgroundColor: "#000000FF";
                            onHoveredChanged: {
                                if (hovered) {
                                    backgroundColor = "#525252";
                                }
                                else
                                    backgroundColor = "#000000FF";
                            }

                            style: ButtonStyle {
                                background: Rectangle {
                                    color: importGamesBtn.backgroundColor;
                                }

                                label: Row {
                                    spacing: 25;
                                    Image {
                                        source: "../assets/folder-8x.png"
                                        height: 40;
                                        width: 40;
                                        sourceSize {
                                            width: 40;
                                            height: 40;
                                        }
                                    }

                                    Column {
                                        anchors.verticalCenter: parent.verticalCenter;
                                        spacing: 2;
                                        Label {
                                            text: "Add Games";
                                            color: "#f1f1f1";
                                            font {
                                                family: "Sans";
                                                pixelSize: 18;
                                                bold: true;
                                            }
                                        }

                                        Label {
                                            text: "Choose your games folder.";
                                            color: "gray";
                                            font {
                                                family: "Sans";
                                                pixelSize: 16;
                                            }
                                        }
                                    }

                                }

                            }
                        }

                        Button {
                            id: importLibrarBtn;
                            property string backgroundColor: "#000000FF";
                            onClicked: {
                                pathDialog.selectFolder = false;
                                pathDialog.title = "Import Library File";
                                pathDialog.nameFilters = ["Library file (*.json)", "All files (*)"];
                                pathDialog.open();
                            }
                            onHoveredChanged: {
                                if (hovered) {
                                    backgroundColor = "#525252";
                                }
                                else
                                    backgroundColor = "#000000FF";
                            }

                            style: ButtonStyle {
                                background: Rectangle {
                                    //opacity: 0.3;
                                    color: importLibrarBtn.backgroundColor;
                                }
                                label: Row {
                                    spacing: 25;
                                    Image {
                                        source: "../assets/file-8x.png";
                                        height: 40;
                                        width: 40;
                                        sourceSize {
                                            width: 40;
                                            height: 40;
                                        }
                                    }

                                    Column {
                                        anchors.verticalCenter: parent.verticalCenter;
                                        spacing: 2;
                                        Label {
                                            text: "Import Library";
                                            color: "#f1f1f1";
                                            font {
                                                family: "Sans";
                                                pixelSize: 18;
                                            }
                                        }
                                        Label {
                                            text: "Add from a Hard Drive.";
                                            color: "gray";
                                            font {
                                                family: "Sans";
                                                pixelSize: 16;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
