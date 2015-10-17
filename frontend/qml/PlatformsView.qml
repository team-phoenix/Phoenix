import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

ScrollView {
    id: platformsView;

    ListView {
        id: listView;
        spacing: 0;
        model: PlatformsModel { id: platformsModel; }

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            anchors.fill: listView.currentItem;

            Rectangle {
                id: highlighterRectangle;
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom; }
                width: 4;
                height: 35;
                color: PhxTheme.common.baseBackgroundColor;
                opacity: .5;
            }
        }

        header: Rectangle {
            color: "transparent";
            height: 70;
            anchors { left: parent.left; right: parent.right; }

            Label {
                text: qsTr( "Systems" );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 17; }
                font { pointSize: PhxTheme.selectionArea.headerFontSize; }
                color: PhxTheme.selectionArea.highlightFontColor;
            }

            /* Button {
                anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 24; }
                text: qsTr( "All" );
                onClicked: { listView.currentIndex = -1; contentArea.contentLibraryModel.clearFilter( "games", "system" ); }
            } */
        }

        delegate: Item {
            height: 30;
            anchors { left: parent.left; right: parent.right; }

            Image {
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin:  17; }
                smooth: true;
                source: "systems/" + listView.model.get( index ) + ".svg";
            }

            Label {
                id: platformText;
                text: listView.model.get( index );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin:  45; }
                font { pointSize: PhxTheme.selectionArea.basePixelSize; }
                color: index === listView.currentIndex ? PhxTheme.common.baseBackgroundColor : PhxTheme.selectionArea.baseFontColor;
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if ( contentArea.contentStackView.currentItem.objectName !== "PlatformsView" ) {
                        contentArea.contentStackView.push( { item: contentArea.boxartGrid, replace: true } );
                    }

                    listView.currentIndex = index;
                    if ( index === 0 ) { contentArea.contentLibraryModel.clearFilter( "games", "system" ); }
                    else { contentArea.contentLibraryModel.setFilter( "games", "system", platformText.text ); }
                }
            }
        }
    }
}
