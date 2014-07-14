import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: settingsWindow;
    width: 500;
    height: 500;

    Item {
        anchors.fill: parent;

        Rectangle {
            id: menuBar;
            color: "#f36363";
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 150;



            ExclusiveGroup {
                id: menuGroup;
                property string buttonBackgroundColor: "#000000FF";
            }

            ColumnLayout {
                id: column;
                anchors {
                    fill: parent;
                    topMargin: 50;
                    bottomMargin: 50;
                }

                spacing: 75;
                property var stacks: {"Input": inputSettings,
                                       "Audio": audioSettings,
                                       "Video": videoSettings,
                                       "Frontend": frontendSettings,
                                       "Save": saveSettings};
                SettingsButton {
                    exclusiveGroup: menuGroup;
                    checked: false;
                    checkable: true;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        //top: parent.top;
                        //topMargin: 25;
                    }

                    implicitHeight: 50;

                    backgroundColor: "#d75858";
                    text: "Input";

                    onCheckedChanged: {
                        if (!checked) {
                            backgroundColor = "#000000FF";
                            //downArrow.source = "";
                        }
                        else {
                            backgroundColor = "#bf4c4c";
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(column.stacks[text]);
                        }
                    }
                }

                SettingsButton {
                    exclusiveGroup: menuGroup;
                    checked: false;
                    checkable: true;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        //top: parent.top;
                        //topMargin: 25;
                    }

                    height: 50;
                    width: parent.width;
                    backgroundColor: "#000000FF";
                    text: "Video";

                    onCheckedChanged: {
                        if (!checked) {
                            backgroundColor = "#000000FF";
                            //downArrow.source = "";
                        }
                        else {
                            backgroundColor = "#bf4c4c";
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(column.stacks[text]);
                        }
                    }
                }


                SettingsButton {
                    exclusiveGroup: menuGroup;
                    checked: false;
                    checkable: true;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        //top: parent.top;
                        //topMargin: 25;
                    }

                    height: 50;
                    width: parent.width;
                    backgroundColor: "#000000FF";
                    text: "Frontend";

                    onCheckedChanged: {
                        if (!checked) {
                            backgroundColor = "#000000FF";
                            //downArrow.source = "";
                        }
                        else {
                            backgroundColor = "#bf4c4c";
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(column.stacks[text]);
                        }
                    }
                }

                SettingsButton {
                    exclusiveGroup: menuGroup;
                    checked: false;
                    checkable: true;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        //top: parent.top;
                        //topMargin: 25;
                    }

                    height: 50;
                    width: parent.width;
                    backgroundColor: "#000000FF";
                    text: "Save";

                    onCheckedChanged: {
                        if (!checked) {
                            backgroundColor = "#000000FF";
                            //downArrow.source = "";
                        }
                        else {
                            backgroundColor = "#bf4c4c";
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(column.stacks[text]);
                        }
                    }
                }

            }
        }


        StackView {
            id: stackView;
            anchors {
                left: menuBar.right;
                right: parent.right;
                bottom: parent.bottom;
                top: parent.top;
            }

            initialItem: videoSettings;
        }

        InnerShadow {
            source: stackView;
            anchors.fill: source;
            radius: 8.0;
            samples: 16;
            horizontalOffset: 4;
            verticalOffset: 3;
            color: Qt.rgba(0, 0, 0, 0.3);
        }

        Component {
            id: videoSettings;
            Rectangle {
                color: "#363434";

                Item {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    height: (parent.height - menuBar.height) - 25;

                    Column {
                        id: column;
                        anchors.fill: parent;
                        spacing: 20;
                        property string textFieldColor: "#1f1f1f";
                        Column {
                            anchors {
                                left: parent.left;
                                right: parent.right;
                                leftMargin: 25;
                                rightMargin: 25;
                            }
                            spacing: 5;
                            Label {
                                text: "Library Path"
                                color: "#f1f1f1";
                                font.bold: true;
                            }

                            TextField {
                                anchors {
                                    left: parent.left;
                                    right: parent.right;
                                    leftMargin: 25;
                                    rightMargin: 25;
                                }

                                placeholderText: "C:/Users/lee/file.end";
                                width: 400;
                                style: TextFieldStyle {
                                    textColor: "#f1f1f1";
                                    placeholderTextColor: textColor;
                                    background: Rectangle {
                                        color: column.textFieldColor;
                                    }
                                }
                            }

                        }

                    }
                }

            }
        }

        Component {
            id: inputSettings;
            Rectangle {
                color: "#363434";

                /*Rectangle{
                    anchors {
                        top: parent.top;
                        right: parent.right;
                    }
                    width: parent.width / 2;
                    height: 50;
                    color: "#232323";

                    Label {
                        anchors.centerIn: parent;
                        text: "Super Nintendo";
                        font.bold: true;
                        font.pixelSize: 11;
                        color: "#f1f1f1";
                    }
                }*/

                Image {
                    id: inputArt;
                    source: "assets/snes-big.png";
                    anchors {
                        bottom: parent.bottom;
                        bottomMargin: 50;
                    }
                    fillMode: Image.PreserveAspectFit;

                    width: parent.width;
                    height: parent.height / 2.5;
                }
            }
        }

        Component {
            id: audioSettings;
            Rectangle {
                color: "#363434";

                Switch {
                    anchors.centerIn: parent;
                    checked: false;
                    style: SwitchStyle {
                        groove: Rectangle {
                            color: control.checked ? "#4cc965" : "#c94c4c";
                            implicitHeight: 20;
                            implicitWidth: 50;
                        }

                        handle: Rectangle {
                            implicitHeight: 20;
                            implicitWidth: 20;
                            color: "#e0d9d9";
                            radius: 2;
                            border.width: 2;
                            border.color: "#d4cecd";
                        }
                    }
                }
            }
        }

        Component {
            id: frontendSettings;
            Rectangle {
                color: "green";
            }
        }

        Component {
            id: saveSettings;
            Rectangle {
                color: "#f1f1f1";
            }
        }
    }




}
