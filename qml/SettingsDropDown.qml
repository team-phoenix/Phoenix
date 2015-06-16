import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.1

Item {
    id: settingsBubble;
    property string groupingColor: "gray";
    property string stackBackgroundColor: "darkgray";
    property string contentColor: "#d7d7d7";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#898989";
    property bool expand: false;
    property bool contentVisible: false;

    Rectangle {
        id: topTriangle;
        visible: parent.visible;
        height: 9;
        width: height;
        rotation: 45;
        color: "#2e2e2e";
        z: settingsBubble.z + 1;
        anchors {
            left: parent.left;
            leftMargin: 17;
            verticalCenter: settingsBubble.top;
        }

        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                leftMargin: 1;
                right: parent.right;
            }
            height: 1;
            color: "#404040";
        }

        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#404040";
        }
    }

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
            color: "#404040";
        }

        CustomBorder {
            color: "black";
            radius: parent.radius;
        }

        color: "#2e2e2e";

        ExclusiveGroup {
            id: menuGroup;
            property string buttonBackgroundColor: "#000000FF";
        }

        ListView {
            id: listView;
            spacing: 0;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: 7;
            }
            interactive: false;
            highlightFollowsCurrentItem: false;
            highlight: Rectangle {
                id: highlightItem;
                anchors.verticalCenter: listView.currentItem.verticalCenter;
                y: listView.currentItem.y;
                color: listView.currentItem ? "#1f1f1f" : "#000000FF";
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#1f1f1f";}
                    GradientStop {position: 0.7; color: "#1f1f1f";}
                    GradientStop {position: 1.0; color: "#141414";}
                }
                anchors {
                    fill: listView.currentItem;
                    rightMargin: 1;
                    leftMargin: 1;
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
                ListElement {title: "Input"; useStack: true; iconSource: "";}
                ListElement {title: "Library"; useStack: true; iconSource: "";}
                ListElement {title: "Save"; useStack: true; iconSource: "";}
                ListElement {title: "Cores"; useStack: true; iconSource: "";}
                ListElement {title: "Advanced"; useStack: true; iconSource: "";}
                ListElement {title: "Video"; useStack: true; iconSource: "";}
                ListElement {title: "Audio"; useStack: true; iconSource: "";}
                ListElement {title: ""; useStack: true; iconSource: "";}
                ListElement {title: "Add Folder..."; useStack: false; iconSource: "";}
                ListElement {title: ""; useStack: true; iconSource: "";}
                ListElement {title: "Close"; useStack: false; iconSource: "";}
            }

            FileDialog {
                id: folderDialog;
                selectFolder: true;
                title: "Add Folder to Library";
                visible: false;
                onAccepted: phoenixLibrary.startAsyncScan(fileUrl);
            }

            delegate: Item {
                height: 24;
                width: parent.width;

                Column {
                    visible: index == 0;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                    }
                    Rectangle {
                        height: 1;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            margins: 1;
                        }
                        color: "#1f1f1f";
                    }
                    Rectangle {
                        height: 1;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                        color: "#424242";
                    }
                }

                Column {
                    anchors {
                        bottom: parent.bottom;
                        left: parent.left;
                        right: parent.right;
                    }
                    Rectangle {
                        height: 1;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                        color: "#1f1f1f";
                    }
                    Rectangle {
                        height: 1;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                        color: "#424242";
                    }
                }



                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onMouseYChanged: listView.currentIndex = index;
                    onClicked: {
                        if (title === listView.currentName) {
                            listView.currentName = "";
                        }
                        else {
                            if (useStack) {
                                settingsWindow.stack.push({item: listView.stacks[title], replace: true, immediate: true});
                                settingsWindow.visible = true;
                            }
                            if (title == "Add Folder...")
                                folderDialog.visible = true;
                            if (title == "Close")
                                close()
                        }
                    }
                }

                Row  {
                    spacing: 8;
                    anchors {
                        left: parent.left;
                        leftMargin: 12;
                        verticalCenter: parent.verticalCenter;
                    }

                    Image {
                        anchors.verticalCenter: parent.verticalCenter;
                        source: iconSource;
                        sourceSize {
                            height: 16;
                            width: 16;
                        }
                    }

                    TextGradient {
                        height: 25;
                        width: listView.width;
                        pointSize: 7;
                        textColor: settingsBubble.textColor;
                        text: title;
                        enableGradient: index === listView.currentIndex;
                        //horizontalAlignment: Text.AlignLeft;
                        //verticalAlignment: Text.AlignVCenter;

                    }
                }
            }
        }
    }
}
