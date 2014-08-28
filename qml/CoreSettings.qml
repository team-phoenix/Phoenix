import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {

    Component.onCompleted: {
        settingsDropDown.width = 500;
    }

    color: settingsBubble.contentColor;

    Column {
        id: coreHeaderColumn;
        spacing: 5;
        anchors {
            left: parent.left;
            top: parent.top;
            topMargin: 25;
            leftMargin: 25;
        }

        Text {
            id: coreTopLabel;
            text: "Core Selection";
            color: settingsBubble.textColor;
            renderType: Text.QtRendering;
            font {
                pixelSize: 18;
            }
        }

        Text {

            text: "Tweak the emulation cores";
            color: settingsBubble.alternateTextColor;
            renderType: Text.QtRendering;
            font {
                pixelSize: 14;
            }
        }

    }



    TableView {
        anchors {
            top: coreHeaderColumn.bottom;
            bottom: bottomBar.top;
            left: parent.left;
            right: parent.right;
            bottomMargin: 15;
            topMargin: 15;
            leftMargin: 25;
            rightMargin: 25;
        }

        model: ListModel {
            ListElement {consoles: "Game Boy";}
            ListElement {consoles: "Game Boy Advance";}
            ListElement {consoles: "Game Boy Color";}
            ListElement {consoles: "Nintendo";}
            ListElement {consoles: "Super Nintendo";}
            ListElement {consoles: "Sega Genesis";}
            ListElement {consoles: "Nintendo 64";}
            ListElement {consoles: "Sony PlayStation";}

        }

        style: TableViewStyle {
            id: tableStyle;
            backgroundColor: settingsBubble.contentColor;

            rowDelegate: Rectangle {
                color: tableStyle.backgroundColor;
                height: 35;
            }

            frame: Item {

            }


            headerDelegate: Item {
                height: 25;
                Text {
                    anchors {
                        left: parent.left;
                    }

                    renderType: Text.QtRendering;
                    color: settingsBubble.textColor;
                    text: styleData.value;
                    font {
                        pixelSize: 16;
                        family: "Sans";
                    }
                }
            }
        }

        TableViewColumn {
            role: "Console";
            title: "Console";
            width: 200;
            delegate: Item {
                anchors.fill: parent;
                CheckBox {
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        right: parent.right;
                        rightMargin: 100;
                    }

                    text: styleData.value;
                    style: CheckBoxStyle {
                        label: Text {
                            text: control.text;
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }
                    }
                }
            }
        }
        TableViewColumn {
            role: "Core";
            title: "Core";
            width: 125;
            delegate: Item {
                anchors.fill: parent;
                ComboBox {
                    height: 25;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        right: parent.right;
                        rightMargin: 15;
                    }

                    model: ["Apple", "Orange"];
                }
            }
        }
        TableViewColumn {
            role: "performance";
            title: "Performance";
            width: 125;
            delegate: Item {
                anchors.fill: parent;
                ComboBox {
                    height: 25;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        right: parent.right;
                        rightMargin: 15;
                    }

                    model: ["Apple", "Orange"];
                }
            }
        }

        TableViewColumn {
            role: "version";
            title: "Version";
            width: 100;
            delegate: Item {
                anchors.fill: parent;
                Text {
                    renderType: Text.QtRendering;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                        right: parent.right;
                        rightMargin: 25;
                    }
                    text: "1.0.0";
                    color: settingsBubble.textColor;
                    font {
                        family: "Sans";
                        pixelSize: 12;
                    }
                }
            }
        }

        TableViewColumn {
            role: "author";
            title: "Author";
            width: 50;
            delegate: Item {
                anchors.fill: parent;
                Text {
                    renderType: Text.QtRendering;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                    }
                    text: "Unknown";
                    color: settingsBubble.textColor;
                    font {
                        family: "Sans";
                        pixelSize: 12;
                    }
                }
            }
        }
    }

    Rectangle {
        id: bottomBar;
        color: settingsBubble.contentColor;
        height: 25;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
            bottomMargin: 25;
        }

        Button {
            id: saveButton;
            text: "Save";
            anchors {
                left: parent.left;
                bottom: parent.bottom;
                leftMargin: 25;
            }
        }

        Button {
           anchors {
               right: parent.right;
               bottom: parent.bottom;
               rightMargin: 25;
           }

           id: defaultButton;
           text: "Default";
        }

    }
}
