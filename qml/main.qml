import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.0
import Qt.labs.settings 1.0

ApplicationWindow {
    id: root;
    width: 640
    height: 480

    title: "Phoenix";

    property bool clear: false;
    property string accentColor:"#b85353";

    Component {
        id: gameGrid;
        GameGrid {
            property string itemName: "grid";
            color: "#1a1a1a";
            zoomFactor: headerBar.sliderValue;
            zoomSliderPressed: headerBar.sliderPressed;

        }
    }

    Component {
        id: gameTable;
        GameTable {
            itemName: "table";
            highlightColor: "#494545";
            textColor: "#f1f1f1";
            headerColor: "#252525";
        }
    }

    Settings {
        id: settings;
        category: "UI";
        property alias windowX: root.x;
        property alias windowY: root.y;
        property alias windowWidth: root.width;
        property alias windowHeight: root.height;
        property alias volumeLevel: gameView.volumeLevel;
    }

   HeaderBar {
        id: headerBar;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }
        height: 50;
        color: "#3b3b3b";
        fontSize: 14;
    }

    DropShadow {
        visible: headerBar.visible;
        source: headerBar;
        anchors.fill: headerBar;
        horizontalOffset: 1;
        verticalOffset: 4;
        radius: 8;
        samples: 16;
        color: "#b0000000"
        transparentBorder: true;
    }

    Item {
        id: settingsBubble;
        z: headerBar.z + 1;
        visible: false;
        focus: false;
        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
            bottomMargin: 30;
            topMargin: 30;
            leftMargin: 20;
        }
        width: parent.width * 0.75;

        SettingsWindow {
            anchors.fill: parent;
            visible: parent.visible;
            stackBackgroundColor: "#4f4f4f";
        }

    }

    DropShadow {
        source: settingsBubble;
        anchors.fill: source;
        horizontalOffset: 4;
        verticalOffset: 8;
        radius: 8.0;
        samples: 16;
        color: "#80000000";
        transparentBorder: true;
    }

   // Second pass needed for left side
   DropShadow {
        source: settingsBubble;
        anchors.fill: source;

        horizontalOffset: -4;
        verticalOffset: 8;
        radius: 8.0;
        samples: 16;
        color: "#80000000"
        transparentBorder: true;
    }

    StackView {
        id: windowStack;
        z: headerBar.z - 1;
        anchors {
            top: headerBar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        initialItem: homeScreen;

        property string gameStackItemName:  {
            if (currentItem != null) {
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
            property string stackName: "";
            property StackView stackId: gameStack;
            ConsoleBar {
                id: consoleBar;
                z: headerBar.z - 1;
                color: "#303030";
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 225;
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
