import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

import "qrc:/Widgets"

// @disable-check M300
PhxScrollView {
    id: platformsView;

    ListView {
        id: listView;
        spacing: 0;
        model: PlatformsModel { id: platformsModel; }

        boundsBehavior: Flickable.StopAtBounds;

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            anchors.fill: listView.currentItem;

            Rectangle {
                id: highlighterRectangle;
                anchors { left: parent.left; top: parent.top; bottom: parent.bottom; }
                width: 4;
                height: PhxTheme.common.menuItemHeight;
                color: PhxTheme.common.menuSelectedColor;
                opacity: .5;
            }
        }

        header: Rectangle {
            color: "transparent";
            height: PhxTheme.common.menuTitleHeight;
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
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            // Image {
            //    anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 17; }
            //    smooth: true;
            //    sourceSize { height: height; width: width; }
            //    source: "systems/" + listView.model.get( index ) + ".svg";
            // }

            MarqueeText {
                id: platformText;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: 45; rightMargin: 17; }
                horizontalAlignment: Text.AlignLeft;

                text: listView.model.get( index );
                fontSize: PhxTheme.selectionArea.basePixelSize;

                color: index === listView.currentIndex ? PhxTheme.common.menuSelectedColor : PhxTheme.selectionArea.baseFontColor;

                spacing: 40;
                running: index === listView.currentIndex || mouseArea.containsMouse;
                pixelsPerFrame: 2.0;
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
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
