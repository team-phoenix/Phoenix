import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

// @disable-check M300
PhxScrollView {
    ListView {
        id: listView;
        spacing: 0;
        model: ListModel {
            id: settingsModel;
            ListElement { section: "Library"; }
            // ListElement { section: "Input (coming soon...)"; }
            // ListElement { section: "Video"; }
            // ListElement { section: "Audio"; }
        }

        boundsBehavior: Flickable.StopAtBounds;

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            anchors.fill: listView.currentItem;

            Rectangle {
                id: highlighterRectangle;
                anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right; }
                height: PhxTheme.common.menuItemHeight;
                color: PhxTheme.common.menuItemBackgroundColor;

                /* Rectangle {
                    anchors { top: parent.top; bottom: parent.bottom; left: parent.left; }
                    width: 4;
                    height: parent.height;
                    color: PhxTheme.common.menuItemHighlight;
                } */
            }
        }

        header: Item {
            height: PhxTheme.common.menuTitleHeight;
            anchors { left: parent.left; right: parent.right; }

            Text {
                text: qsTr( "Settings" );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: PhxTheme.common.menuItemMargin; }
                font { pixelSize: PhxTheme.selectionArea.headerFontSize; }
                color: PhxTheme.selectionArea.highlightFontColor;
            }
        }

        delegate: Item {
            id: listViewDelegate;
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            MarqueeText {
                id: sectionText;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: PhxTheme.common.menuItemMargin; rightMargin: PhxTheme.common.menuItemMargin; }
                horizontalAlignment: Text.AlignLeft;

                text: section;
                fontSize: PhxTheme.common.baseFontSize;
                color: index === listView.currentIndex ? PhxTheme.common.baseBackgroundColor : PhxTheme.selectionArea.baseFontColor;

                spacing: 40;
                running: index === listView.currentIndex || mouseArea.containsMouse;
                pixelsPerFrame: 2.0;
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    listView.currentIndex = index
                    switch ( section ) {
                    case "Library":
                        if( contentArea.contentStackView.currentItem.objectName !== "LibrarySettingsView") {
                            contentArea.contentStackView.push( { item: contentArea.contentLibrarySettingsView, replace: true } );
                        }
                        break;
                    case "Input":
                        if( contentArea.contentStackView.currentItem.objectName !== "InputSettingsView") {
                            // contentArea.contentStackView.push( { item: contentArea.contentInputSettingsView, replace: true } );
                        }
                        break;
                    default:
                        break;
                    }
                }
            }

        }
    }
}

