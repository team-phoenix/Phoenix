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

    property bool clear: true;
    property string accentColor:"#b85353";

    Settings {
        category: "UI";
        property alias windowX: root.x;
        property alias windowY: root.y;
        property alias windowWidth: root.width;
        property alias windowHeight: root.height;
        property alias volumeLevel: gameView.volumeLevel;
    }

    StackView {
        id: windowStack;
        anchors.fill: parent;
        initialItem: homeScreen;
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
            HeaderBar {
                id: headerBar;
                z: gameStack.z + 1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                }
                height: 50;
                color: "#666666";
                fontSize: 14;

            }

            ConsoleBar {
                id: consoleBar;
                //z: headerBar.z - 1;
                z: gameStack.z + 1;
                color: "#333333";
                anchors {
                    left: parent.left;
                    top: headerBar.bottom;
                    bottom: parent.bottom;
                }
                width: 275;
            }

            StackView {
                id: gameStack;
                initialItem: {
                    if (root.clear === true)
                        return emptyScreen;

                    return gameGrid;
                }


                anchors {
                    left: consoleBar.right;
                    right: parent.right;
                    top: headerBar.bottom;
                    bottom: parent.bottom;
                }

            }

            InnerShadow {
                source: gameStack;
                anchors.fill: source;
                radius: 8.0;
                samples: 16;
                horizontalOffset: 6;
                verticalOffset: 1;
                color: Qt.rgba(0, 0, 0, 0.3);
            }

            Component {
                id: gameGrid;
                GameGrid {
                    property string itemName: "grid";
                    color: "#191a1a";
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
                    headerColor: "#2c2c2c";
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
