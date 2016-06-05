import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import vg.phoenix.backend 1.0
import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.scanner 1.0

import "../"
import "qrc:/Assets"
import "qrc:/Util"
import "qrc:/Frontend"
import "qrc:/Settings"

Rectangle {
    property Item currentItem;

    // All 3 lists the sidebar can show
    // Only one is enabled and visible at any given time
    // Set with currentItem
    Item {
        anchors.top: parent.top;
        anchors.bottom: bottomRowContainer.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        SystemList {
            id: systemList;

            width: parent.width;
            height: parent.height;

            enabled: opacity === 1.0;

            opacity: currentItem === this ? 1.0 : 0.0;
            Behavior on opacity { PropertyAnimation { } }
            y: currentItem === this ? 0 : parent.height;
            Behavior on y { PropertyAnimation { easing.type: Easing.InOutExpo; } }
        }

        CollectionsView {
            id: collectionsView;

            width: parent.width;
            height: parent.height;

            enabled: opacity === 1.0;

            opacity: currentItem === this ? 1.0 : 0.0;
            Behavior on opacity { PropertyAnimation { } }
            y: currentItem === this ? 0 : parent.height;
            Behavior on y { PropertyAnimation { easing.type: Easing.InOutExpo; } }
        }

        SettingsList {
            id: settingsList;

            width: parent.width;
            height: parent.height;

            enabled: opacity === 1.0;

            opacity: currentItem === this ? 1.0 : 0.0;
            Behavior on opacity { PropertyAnimation { } }
            y: currentItem === this ? 0 : parent.height;
            Behavior on y { PropertyAnimation { easing.type: Easing.InOutExpo; } }
        }
    }

    Item {
        id: bottomRowContainer;
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; }
        height: bottomRowHeight + ( running ? scanProgressHeight : 0 );

        Behavior on height { NumberAnimation { easing.type: Easing.InOutExpo; duration: 1000; } }

        property int scanProgressHeight: 50;
        property int bottomRowHeight: 65;
        property bool running: GameHasherController.running;

        // A progress indicator for the game scanner
        Rectangle {
            anchors { left: parent.left; right: parent.right; }
            color: PhxTheme.common.tertiaryBackgroundColor;
            width: 250;
            height: bottomRowContainer.scanProgressHeight;
            y: parent.running ? 0 : bottomRowContainer.bottomRowHeight;

            Behavior on y { NumberAnimation { easing.type: Easing.InOutExpo; duration: 1000; } }

            Rectangle {
                anchors { top: parent.top; right: parent.right; left: parent.left; }
                color: "transparent";
                width: parent.width;
                height: parent.height/2;

                // RESUME - PAUSE
                /*
                Rectangle {
                    color: "transparent";
                    width: 20;
                    height: 20;

                    Image {
                        anchors { centerIn: parent; }
                        width: 10;
                        height: 10;
                        source: GameHasherController.paused ? "resume.svg" : "pause2.svg";
                        ourceSize { width: width; height: height; }
                     }

                     MouseArea {
                        anchors.fill: parent;
                        onClicked: { GameHasherController.paused ? GameHasherController.resume() : GameHasherController.pause(); }
                    }
                }
                */

                // CANCEL
                Rectangle {
                    anchors { top: parent.top; right: parent.right; topMargin: 8; rightMargin: 8; }
                    color: "transparent";
                    width: 8;
                    height: width;

                    Image {
                        anchors.centerIn: parent;
                        width: parent.width;
                        height: parent.height;
                        source: "qrc:/Assets/close.svg";
                        sourceSize { width: width; height: height; }
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: { GameHasherController.cancel(); }
                    }
                }

                Text {
                    anchors.centerIn: parent;
                    text: "Importing Games";
                    color: "white";
                }
            }

            Rectangle {
                anchors { bottom: parent.bottom; right: parent.right; left: parent.left; }
                color: "transparent";
                width: parent.width;
                height: parent.height / 2;

                ProgressBar {
                    anchors { left: parent.left; right: parent.right; leftMargin: 25; rightMargin: 25; }
                    width: parent.width * 0.6;
                    height: 6;
                    value: GameHasherController.progress;
                    minimumValue: 0;
                    maximumValue: 500;

                    style: ProgressBarStyle {
                        background: Rectangle { color: "white"; border.width: 0; }
                        progress: Rectangle { color: PhxTheme.common.menuItemHighlight; border.width: 0; }
                    }
                }
            }
        }

        Rectangle {
            id: listViewContainer;
            height: bottomRowContainer.bottomRowHeight;
            color: PhxTheme.common.primaryBackgroundColor;
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom; }

            // The "+ 1" represents the number of extra buttons to the right of the ListView
            property real delegateWidth: parent.width / ( listView.count + 1 );

            // The buttons along the bottom that control which list is currently visible
            PhxListView {
                id: listView;
                anchors { bottom: parent.bottom; left: parent.left; }
                width: parent.width - parent.delegateWidth;
                height: bottomRowContainer.bottomRowHeight;
                spacing: 0;
                orientation: ListView.Horizontal;

                // FIXME: Use ids/properties as leftPane/rightPane roles once QTBUG-44079 is fixed
                model: ListModel {
                    ListElement { bgColor: "white"; label: "Games"; imageSource: "qrc:/Assets/games.svg"; }
                    ListElement { bgColor: "white"; label: "Favorites"; imageSource: "qrc:/Assets/collections.svg"; }
                    ListElement { bgColor: "white"; label: "Settings"; imageSource: "qrc:/Assets/settings.svg"; }
                }

                delegate: Item {
                    anchors { top: parent.top; bottom: parent.bottom; }
                    width: listViewContainer.delegateWidth;

                    Image {
                        anchors { centerIn: parent; }
                        width: 24;
                        height: 24;

                        source: imageSource;
                        sourceSize { width: width; height: height; }

                        opacity: index === listView.currentIndex ? 1.0 : 0.5;
                    }

                    // If the index now points to this particular delegate, change the current list accordingly
                    Connections {
                        target: listView;
                        onCurrentIndexChanged: {
                            if( listView.currentIndex === index ) {
                                // FIXME: Remove this once QTBUG-44079 is fixed (check ListModel above)
                                switch( index ) {
                                case 0:
                                    currentItem = systemList;
                                    mainArea.currentItem = library;
                                    break;
                                case 1:
                                    currentItem = collectionsView;
                                    mainArea.currentItem = library;
                                    break;
                                case 2:
                                    currentItem = settingsList;
                                    settingsList.indexChanged( settingsList.currentIndex );
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: listView.currentIndex = index;
                    }
                }
            }

            Rectangle {
                id: addButtonHighlight;
                width: parent.delegateWidth;
                anchors { top: listView.top; bottom: listView.bottom; left: listView.right; }

                color: "transparent";
                border.color: activeFocus || fileDialog.visible ? PhxTheme.common.menuItemHighlight : "transparent";
                border.width: 2;

                activeFocusOnTab: true;
                Keys.onPressed: {
                    if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                        fileDialog.open();
                        event.accepted = true;
                    }
                }

                FileDialog {
                    id: fileDialog;
                    selectMultiple: true;
                    title: "Select games to import into your library"
                    onAccepted: { GameHasherController.scanForGames( fileUrls ); }
                }

                Image {
                    id: addGameButton;
                    anchors { centerIn: parent; }
                    width: 24;
                    height: 24;

                    source: "qrc:/Assets/add.svg";
                    sourceSize { width: width; height: height; }

                    opacity: fileDialog.visible ? 1.0 : 0.5;

                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        addButtonHighlight.focus = true;
                        fileDialog.open();
                    }
                }
            }
        }
    }
}
