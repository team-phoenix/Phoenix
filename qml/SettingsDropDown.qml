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
            highlight: Rectangle {
                id: highlightItem;
                height: listView.currentItem.height;
                width: listView.width;
                anchors.verticalCenter: listView.currentItem.verticalCenter;
                y: listView.currentItem.y;
                color: listView.currentItem ? "#525252" : "#000000FF";
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
                ListElement {title: "Add Folder"; useStack: false; iconSource: "";}
            }

            FileDialog {
                id: folderDialog;
                selectFolder: true;
                title: "Add Folder to Library";
                visible: false;
                onAccepted: phoenixLibrary.startAsyncScan(fileUrl);
            }

            delegate: Item {
                height: 25;
                width: parent.width;
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
                            if (title == "Add Folder")
                                folderDialog.visible = true;
                        }
                    }
                }

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
                            pixelSize: 11;
                        }

                        color: settingsBubble.textColor;
                        text: title;
                        horizontalAlignment: Text.AlignLeft;
                        verticalAlignment: Text.AlignVCenter;

                    }
                }
            }
        }
    }
}
