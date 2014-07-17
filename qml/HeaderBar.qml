import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1

Rectangle {
    id: headerBar;
    property string headerColor: "#404040";
    property int fontSize: 13;

    property real sliderValue: 0;
    property bool sliderPressed: false;

    width: 300;
    height: 50;
    color: headerColor;

    SettingsWindow {
        id: settingsWindow;
        visible: false;
        width: 500;
        height: 500;
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
                    backgroundColor = "#525252";
                }
                else
                    backgroundColor = "#000000FF"
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
            onPressedChanged:  {
                settingsWindow.visible = true;
            }

        }

        FileDialog {
            id: folderDialog;
            selectFolder: true;
            title: "Add Folder to Library";
            visible: false;

        }

        Button {
            id: folderBtn;
            property string backgroundColor: "#000000FF";
            height: 30;
            width: 30;
            onHoveredChanged: {
                if (hovered) {
                    backgroundColor = "#525252";
                }
                else
                    backgroundColor = "#000000FF"
            }
            style: ButtonStyle {
                background: Rectangle {
                    color: folderBtn.backgroundColor;
                }

                label: Image{
                    source: "../assets/folder-8x.png";
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
            id: viewBtn;
            height: 30;
            width: 30;
            property string backgroundColor: "#000000FF";
            property string imageSource: "../assets/grid-three-up-8x.png";
            onHoveredChanged: {
                if (hovered) {
                    backgroundColor = "#525252";
                }
                else
                    backgroundColor = "#000000FF"
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
                    if (gameStack.get(0, false).itemName === "grid") {
                        imageSource = "../assets/list-8x.png";
                        gameStack.clear();
                        gameStack.push(gameTable);
                    }
                    else {
                        imageSource = "../assets/grid-three-up-8x.png";
                        gameStack.clear();
                        gameStack.push(gameGrid);
                    }


                }
                else
                    backgroundColor = "#000000FF";
            }

        }

        Slider {
            width: 150;
            height: 5;
            anchors {
                verticalCenter: parent.verticalCenter;
            }
            stepSize: 2;
            maximumValue: 100;
            minimumValue: -100;
            value: 0;

            onPressedChanged: {
                if (pressed)
                    headerBar.sliderPressed = true;
                else
                    headerBar.sliderPressed = false;
            }

            onValueChanged: headerBar.sliderValue = value;
            style: SliderStyle {
                handle: Rectangle {
                    height: 18;
                    width: 18;
                    color: "#f1f1f1";
                    radius: 8;
                }

                groove: Rectangle {
                    implicitHeight: 5;
                    implicitWidth: 200;
                    radius: 2;
                    opacity: 0.8;
                    color: "#333333";
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

        onTextChanged: {
            gamelibrary.setFilter(text);
        }

        style: TextFieldStyle {
            placeholderTextColor: "#f1f1f1";

            background: Rectangle {
                radius: 2;
                opacity: 0.8;
                color: "#333333";

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
