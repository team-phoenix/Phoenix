import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

ScrollView {
    width: 100;
    height: 62;

    ListView {
        id: listView;
        spacing: 0;
        model: ListModel {
            id: settingsModel;
            ListElement { section: "Coming soon..."; }
            // ListElement { section: "Input"; }
            // ListElement { section: "Video"; }
            // ListElement { section: "Audio"; }
        }

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            anchors.fill: listView.currentItem;

            Rectangle {
                id: highlighterRectangle;
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom; }
                width: 5;
                height: 35;
                color: PhxTheme.common.baseBackgroundColor;
                opacity: .5;
            }
        }

        header: Item {
            height: 70;
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
            height: 35;
            anchors { left: parent.left; right: parent.right; }

            Text {
                id: sectionText;
                text: section;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 45; }
                font { pointSize: PhxTheme.selectionArea.basePixelSize; }
                color: index === listView.currentIndex ? PhxTheme.common.baseBackgroundColor : PhxTheme.selectionArea.baseFontColor;
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index
                    switch ( index ) {
                    case 0:
                        // contentArea.contentStackView.push( { item: contentArea.contentInputView, replace: true } );
                        break;
                    case 1:
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

