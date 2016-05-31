import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

import "qrc:/Frontend"
import "qrc:/Settings"

Rectangle {
    color: PhxTheme.common.secondaryBackgroundColor;

    property alias mainArea: mainArea;

    MouseArea {
        anchors.fill: parent;

        RowLayout {
            anchors.top: parent.top;
            anchors.bottom: minimizedGame.top;
            width: parent.width;
            spacing: 0;

            // A sidebar for manipulating the entire frontend
            Sidebar {
                id: sidebar;
                anchors { top: parent.top; bottom: parent.bottom; }
                width: PhxTheme.common.menuWidth;
                clip: true;

                // Place above the main area
                z: 1;

                color: PhxTheme.common.primaryBackgroundColor;
            }

            Item {
                id: mainArea;
                anchors { top: parent.top; bottom: parent.bottom; }
                Layout.fillWidth: true;

                property Item currentItem;

                Item {
                    id: library;
                    property alias libraryModel: libraryComponent.libraryModel;

                    width: parent.width;
                    height: parent.height;

                    enabled: opacity === 1.0;

                    opacity: mainArea.currentItem === this ? 1.0 : 0.0;
                    Behavior on opacity { PropertyAnimation { } }
                    x: mainArea.currentItem === this ? 0 : -parent.width;
                    Behavior on x { PropertyAnimation { easing.type: Easing.InOutExpo; } }

                    ColumnLayout {
                        anchors.fill: parent;
                        spacing: 0;

                        LibraryHeader {
                            id: header;
                            anchors { left: parent.left; right: parent.right; }
                            height: 70;
                        }

                        Library {
                            id: libraryComponent;
                            Layout.fillHeight: true;
                            anchors { left: parent.left; right: parent.right; }

                            property alias header: header;
                            property alias searchBar: header.searchBar;
                            property alias zoomSlider: header.zoomSlider;
                        }
                    }
                }

                LibretroCoreSettings {
                    id: libretroCoreSettings;

                    width: parent.width;
                    height: parent.height;

                    enabled: opacity === 1.0;

                    opacity: mainArea.currentItem === this ? 1.0 : 0.0;
                    Behavior on opacity { PropertyAnimation { } }
                    x: mainArea.currentItem === this ? 0 : -parent.width;
                    Behavior on x { PropertyAnimation { easing.type: Easing.InOutExpo; } }
                }

                InputSettings {
                    id: inputSettings;

                    width: parent.width;
                    height: parent.height;

                    enabled: opacity === 1.0;

                    opacity: mainArea.currentItem === this ? 1.0 : 0.0;
                    Behavior on opacity { PropertyAnimation { } }
                    x: mainArea.currentItem === this ? 0 : -parent.width;
                    Behavior on x { PropertyAnimation { easing.type: Easing.InOutExpo; } }
                }
            }
        }

        // Wrap MinimizedGame with an Item so we can squash the Item without resizing MinimizedGame's contents
        Item {
            id: minimizedGame;
            property bool showMinimizedGame: phoenix.state === "Minimizing" || phoenix.state === "Minimized";
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
            height: showMinimizedGame ? 65 : 0;
            clip: true;
            Behavior on height { PropertyAnimation { duration: 250; } }

            MinimizedGame {
                anchors { top: parent.top; left: parent.left; right: parent.right; }
                height: 65;
            }
        }
    }
}
