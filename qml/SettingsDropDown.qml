import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Item {
    id: settingsBubble;
    property string groupingColor: "gray";
    property string stackBackgroundColor: "darkgray";
    property string contentColor: "#d7d7d7";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#898989";
    property bool expand: false;

    property bool contentVisible: false;

    SettingsWindow {
        id: settingsWindow;
        visible: false;
    }

    Rectangle {
        id: menuBar;
        //color: settingsBubble.stackBackgroundColor;
        radius: 3;
        anchors {
            fill: parent;
        }

        border {
            width: 1;
            color: "#333333";
        }

        //gradient: Gradient {
         //   GradientStop {position: 0.0; color: "#30302f";}
        //    GradientStop {position: 1.0; color: "#242424";}
        //}
        color: "#2f2f2f";

        ExclusiveGroup {
            id: menuGroup;
            property string buttonBackgroundColor: "#000000FF";
        }

        ListView {
            id: listView;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: 15;
            }
            interactive: false;
            highlightFollowsCurrentItem: false;
            highlight: Item {
                id: highlightItem;
                height: listView.currentItem.height;
                width: listView.width;
                anchors.verticalCenter: listView.currentItem.verticalCenter;
                y: listView.currentItem.y;

                //Component.onCompleted: visible = false;

                Item {
                    id: innerItem;
                    height: parent.height;
                    width: parent.width;

                    Rectangle {
                        id: topBorder;
                        z: mainColor.z + 1;
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            leftMargin: menuBar.border.width;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#262625";
                    }

                    Rectangle {
                        anchors {
                            right: parent.left;
                            rightMargin: menuBar.border.width;
                            top: topBorder.bottom;
                            bottom: bottomBorder.top;
                        }
                        width: 1;
                        color: "#212121";
                    }

                    Rectangle {
                        anchors {
                            left: parent.left;
                            leftMargin: menuBar.border.width;
                            top: topBorder.bottom;
                            bottom: bottomBorder.top;
                        }
                        width: 1;
                        color: "#212121";
                    }

                    Rectangle {
                        id: mainColor;
                        anchors {
                            left: parent.left;
                            leftMargin: menuBar.border.width;
                            right: parent.right;
                            top: parent.top;
                            bottom: parent.bottom;
                        }
                        color: listView.currentItem ? "#171717" : "#000000FF";
                    }

                    Rectangle {
                        // bottomBorder;
                        id: bottomBorder;
                        z: topBorder.z;
                        anchors {
                            bottom: parent.bottom;
                            left: parent.left;
                            leftMargin: menuBar.border.width;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#474747";
                    }

                }
            }

            property var stacks: { "Input": settingsWindow.input,
                                   "Cores": settingsWindow.core,
                                   "Library": settingsWindow.library,
                                   "Save": settingsWindow.save,
                                   "Video": settingsWindow.video,
                                   "Audio": settingsWindow.audio,
                                   "Advanced": settingsWindow.advanced};

            property string currentName: "";

            model: ListModel {
                ListElement {title: "Input"; iconSource: "../assets/Controls-64.png";}
                ListElement {title: "Library"; iconSource: "";}
                ListElement {title: "Save"; iconSource: "";}
                ListElement {title: "Cores"; iconSource: "../assets/Core-32.png";}
                ListElement {title: "Advanced"; iconSource: "";}
                ListElement {title: "Video"; iconSource: "";}
                ListElement {title: "Audio"; iconSource: "";}

            }

            delegate: Item {
                height: 25;
                Row  {
                    spacing: 8;
                    anchors {
                        left: parent.left;
                        leftMargin: 24;
                    }

                    Image {
                        anchors.verticalCenter: parent.verticalCenter;
                        source: iconSource;
                        sourceSize {
                            height: 16;
                            width: 16;
                        }
                    }

                    Text {
                        height: 25;
                        renderType: Text.QtRendering;
                        font {
                            family: "Sans";
                            pixelSize: 12;
                        }

                        color: settingsBubble.textColor;
                        text: title;
                        horizontalAlignment: Text.AlignLeft;
                        verticalAlignment: Text.AlignVCenter;

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                if (title === listView.currentName) {
                                    listView.currentName = "";
                                }
                                else {
                                    listView.currentIndex = index;
                                    settingsWindow.stack.push({item: listView.stacks[title], replace: true, immediate: true});
                                    settingsWindow.visible = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
