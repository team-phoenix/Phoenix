import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

// @disable-check M300
PhxScrollView {
    width: 100;
    height: 62;

    ListView {
        id: listView;
        spacing: 0;
        model: ListModel {
            id: settingsModel;
            ListElement { section: "Input (coming soon...)"; }
            ListElement { section: "Library"; }
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
                color: Qt.rgba(255,2555,255,.1);

                Rectangle {
                    anchors { top: parent.top; bottom: parent.bottom; left: parent.left; }
                    width: 4;
                    height: parent.height;
                    color: PhxTheme.common.menuItemHighlight;
                }
            }
        }

        header: Item {
            height: PhxTheme.common.menuTitleHeight;
            anchors { left: parent.left; right: parent.right; }

            Text {
                text: qsTr( "Settings" );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 17; }
                font { pointSize: PhxTheme.selectionArea.headerFontSize; }
                color: PhxTheme.selectionArea.highlightFontColor;
            }
        }

        delegate: Item {
            id: listViewDelegate;
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            MarqueeText {
                id: sectionText;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: 45; rightMargin: 17; }
                horizontalAlignment: Text.AlignLeft;

                text: section;
                fontSize: PhxTheme.selectionArea.basePixelSize;
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
                    switch ( index ) {
                    case 0:
                        // contentArea.contentStackView.push( { item: contentArea.contentInputView, replace: true } );
                        break;
                    case 1:
                        contentArea.contentStackView.push( { item: contentArea.contentLibraryView, replace: true } );
                        break;
                    case 2:
                        break;
                    default:
                        break;
                    }
                }
            }

        }
    }
}

