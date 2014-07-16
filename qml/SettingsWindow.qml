import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: settingsWindow;
    width: 500;
    height: 500;

    minimumWidth: 500;
    minimumHeight: 500;
    maximumHeight: 500;
    maximumWidth: 500;

    Item {
        anchors.fill: parent;

        Rectangle {
            id: menuBar;
            z: stackView.z + 1;
            color: "#333333";
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 150;



            ExclusiveGroup {
                id: menuGroup;
                property string buttonBackgroundColor: "#000000FF";
            }



            ListView {
                id: listView;
                anchors {
                    top: parent.top;
                    bottom: advancedRectangle.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 20;
                    //bottomMargin: 50;
                }
                highlightFollowsCurrentItem: false;
                spacing: 15;
                highlight: Item {
                    id: highlightItem;
                    height: listView.currentItem.height;
                    width: listView.width;
                    anchors.verticalCenter: listView.currentItem.verticalCenter;
                    y: listView.currentItem.y;
                    Item {
                        id: innerItem;
                        height: parent.height;
                        width: parent.width;
                        Rectangle {
                            id: accentRectangle;
                            color: root.accentColor;
                            width: 15;
                            anchors {
                                left: parent.left;
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                        }


                        Rectangle {
                            id: mainColor;
                            anchors {
                                left: accentRectangle.right;
                                right: parent.right;
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                            color: listView.currentItem ? "#666666" : "#000000FF";
                        }


                        Image {
                            id: image;
                            anchors {
                                left: mainColor.right;
                            }
                            source: "../assets/triangle.png";
                            sourceSize {
                                height: 20;
                                width: 30;
                            }
                            height: highlightItem.height;
                            width: 30;
                        }

                    }

                    DropShadow {
                        width: innerItem.width + 30;
                        height: innerItem.height + 5;

                        horizontalOffset: 1;
                        verticalOffset: 1;
                        radius: 8.0
                        samples: 16
                        transparentBorder: true;
                        color: "#80000000"
                        opacity: 0.8;
                        source: innerItem;
                    }
                }

                property var stacks: { "Input": inputSettings,
                                       "Cores": coreSettings,
                                       "Info": infoSettings,
                                       "Library": frontendSettings,
                                       "Save": saveSettings};
                model: ListModel {
                    ListElement {title: "Input";}
                    //ListElement {title: "System";}
                    ListElement {title: "Library";}
                    ListElement {title: "Save";}
                    ListElement {title: "Cores";}
                    ListElement {title: "Advanced";}
                    //ListElement {title: "Developer";}
                    ListElement {title: "Themes";}


                }

                delegate: Label {
                    height: 35;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        //top: parent.top;
                        //topMargin: 25;
                    }
                    font {
                        family: "Sans";
                        bold: true;
                        pixelSize: 16;
                    }

                    color: "#f1f1f1";
                    text: title;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            //downArrow.source = "";
                            listView.currentIndex = index;
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(listView.stacks[text]);
                        }
                    }
                }
            }

            Rectangle {
                id: advancedRectangle;
                z: listView.z + 1;
                height: 75;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }

                color: "#242323";


                Image {
                    anchors.centerIn: parent;
                    source: "../assets/more.png";
                    sourceSize {
                        width: 40;
                        height: 40;
                    }
                    height: 40;
                    width: 40;
                }
            }

            DropShadow {
                source: advancedRectangle;
                anchors.fill: source;
                horizontalOffset: 0;
                verticalOffset: -3;
                radius: 8.0
                samples: 16
                transparentBorder: true;
                color: "#80000000"
                //opacity: 0.8;
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

            initialItem: infoSettings;
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
            id: infoSettings;
            Rectangle {
                color: "#555555";

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
                    source: "../assets/snes-big.png";
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
            id: coreSettings;
            Rectangle {
                color: "#404040";

                Row {
                    id: btnRow;
                    spacing: 2;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        top: parent.top;
                        topMargin: 25;
                    }

                    ExclusiveGroup {
                        id: coreBtnGroup;
                    }

                    Button {
                        id: configBtn;
                        width: 100;
                        height: 35;
                        checkable: true;
                        checked: false;
                        exclusiveGroup: coreBtnGroup;

                        property string backgroundColor: checked ? "#383838" : "#4a4a4a";

                        onClicked: {
                            coreStackView.clear();
                            coreStackView.push(coreOptions);
                        }

                        style: ButtonStyle {
                            background: Rectangle {
                                color: configBtn.backgroundColor;
                            }
                            label: Label {
                                text: "Configure";
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 18;
                                    bold: true;
                                }
                                verticalAlignment: Text.AlignVCenter;

                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }

                    Button {
                        id: infoBtn;
                        width: 100;
                        height: 35;
                        checkable: true;
                        checked: true;
                        exclusiveGroup: coreBtnGroup;
                        property string backgroundColor: checked ? "#383838" : "#4a4a4a";

                        onClicked: {
                            coreStackView.clear();
                            coreStackView.push(coreTable);
                        }

                        style: ButtonStyle {
                            background: Rectangle {
                                color: infoBtn.backgroundColor;
                            }
                            label: Label {
                                text: "Info";
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 18;
                                    bold: true;
                                }
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter;
                            }
                        }
                    }
                }

                StackView {
                    id: coreStackView;
                    anchors {
                        top: btnRow.bottom;
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: 25;
                    }
                    initialItem: coreTable;

                }

                Component {
                    id: coreTable;
                    TableView {
                        TableViewColumn{ role: "title"  ; title: "Title" ; width: 100 }
                        TableViewColumn{ role: "author" ; title: "Author" ; width: 200 }
                            model: ListModel {
                                ListElement{title: "A Masterpiece"; author: "Gabriel";}
                                ListElement{title: "Brilliance"; author: "Jens";}
                                ListElement{title: "Outstanding"; author: "Frederik";}
                       }
                    }

                }

                Component {
                    id: coreOptions;
                    Rectangle {
                        color: "blue";

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
