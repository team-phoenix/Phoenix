import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

import "../"
import "../Util"
import "../Theme"

PhxScrollView {
    // Change to reflect index 0 of settingsModel
    property Item defaultItem: libretroCoreSettings;
    property int currentIndex: listView.currentIndex;

    function indexChanged( index ) {
        switch( index ) {
            case 0:
                mainArea.currentItem = libretroCoreSettings;
                break;
            case 1:
                mainArea.currentItem = inputSettings;
                break;
            default:
                break;
        }
    }

    PhxListView {
        id: listView;
        anchors.fill: parent;

        spacing: 0;

        model: ListModel {
            id: settingsModel;
            ListElement { section: "Library"; }
            ListElement { section: "Input"; }
            // ListElement { section: "Video"; }
            // ListElement { section: "Audio"; }
        }

        header: Item {
            height: PhxTheme.common.menuTitleHeight;
            anchors { left: parent.left; right: parent.right; }

            Text {
                text: "Settings";
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

                onClicked: listView.currentIndex = index;
            }

            // If the index now points to this particular delegate, change the current list accordingly
            Connections {
                target: listView;
                onCurrentIndexChanged: {
                    if( listView.currentIndex === index && sidebar.currentItem === settingsList ) {
                        indexChanged( index );
                    }
                }
            }
        }
    }
}
