import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.0

/*import Library 1.0*/

ApplicationWindow {
    id: root;
    width: 640
    height: 480
    title: "Phoenix";

    property bool clear: true;
    property string accentColor:"#b85353";

/*    Library {
        id: library;
        onCountChanged: console.log(count);
        Component.onCompleted: {
            var obj = loadLibrary();

        }
    }*/

    HeaderBar {
        id: headerBar;
        z: gameStack.z + 1;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }
        height: 50;
        color: "#404040";
        fontSize: 14;

    }

    ConsoleBar {
        id: consoleBar;
        //z: headerBar.z - 1;
        z: gameStack.z + 1;
        color: "#2b2b2b";
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
        horizontalOffset: 3;
        verticalOffset: 1;
        color: Qt.rgba(0, 0, 0, 0.3);
    }

    Component {
        id: gameGrid;
        GameGrid {
            property string itemName: "grid";
            color: "#1d1e1e";
            sliderValue: headerBar.sliderValue;
            sliderPressed: headerBar.sliderPressed;

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
                spacing: 80;

                FileDialog {
                    id: pathDialog;
                    selectFolder: true;
                    title: "Add Game Folder";
                    onAccepted: {
                        library.scanFolder(fileUrl.toString().replace("file:///", ""));
                    }
                }

                Button {
                    onClicked: pathDialog.open();

                    style: ButtonStyle {
                        background: Rectangle {
                            opacity: 0.3;
                            color: control.pressed ? "#111111" : "#000000FF";
                        }

                        label: Row {
                            spacing: 25;
                            Image {
                                source: "/assets/folder-8x.png"
                                sourceSize {
                                    width: 50;
                                    height: 50;
                                }
                            }
                            Label {
                                anchors.verticalCenter: parent.verticalCenter;
                                text: "Add Games";
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 16;
                                    bold: true;
                                }
                            }

                        }

                    }
                }

                Button {
                    onClicked: {
                        pathDialog.selectFolder = false;
                        pathDialog.title = "Import Library File";
                        pathDialog.nameFilters = ["Library file (*.json)", "All files (*)"];
                        pathDialog.open();
                    }

                    style: ButtonStyle {
                        background: Rectangle {
                            opacity: 0.3;
                            color: control.pressed ? "#111111" : "#000000FF";
                        }
                        label: Row {
                            spacing: 25;
                            Image {
                                source: "/assets/file-8x.png"
                                sourceSize {
                                    width: 50;
                                    height: 50;
                                }
                            }
                            Label {
                                anchors.verticalCenter: parent.verticalCenter;
                                text: "Import Library";
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 16;
                                    bold: true;
                                }
                            }

                        }

                    }
                }

            }
        }
    }
}
