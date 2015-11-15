import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

// @disable-check M300
PhxScrollView {

    // @disable-check M300
    PhxListView {
        id: listView;
        anchors.fill: parent;

        spacing: 0;

        model: ListModel {
            id: settingsModel;
            ListElement { section: "Library"; }
            // ListElement { section: "Input (coming soon...)"; }
            // ListElement { section: "Video"; }
            // ListElement { section: "Audio"; }
        }

        highlightFollowsCurrentItem: false;

        signal doShowAnimation();

        highlight: Rectangle {
            id: highlighter;
            width: 4;
            height: listView.currentItem.height;
            color: PhxTheme.common.menuItemHighlight;

            x: 0;
            y: 0;

            Connections {
                target: listView;
                onDoShowAnimation: {
                    showAnimation.complete();
                    showAnimation.start();
                }
            }

            SequentialAnimation {
                id: showAnimation;
                PropertyAction { target: highlighter; properties: "y"; value: listView.currentItem.y; }
                PropertyAnimation { target: highlighter; properties: "x"; from: -4; to: 0; duration: 300; easing.type: Easing.InOutQuart; }
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
                            listView.doShowAnimation();
                        }
                        break;
                    case "Input":
                        if( contentArea.contentStackView.currentItem.objectName !== "InputSettingsView") {
                            // contentArea.contentStackView.push( { item: contentArea.contentInputSettingsView, replace: true } );
                            listView.doShowAnimation();
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

