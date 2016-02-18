import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

// @disable-check M300
PhxScrollView {
    id: platformsView;

    // @disable-check M300
    PhxListView {
        id: listView;
        anchors.fill: parent;

        spacing: 0;
        model: PlatformsModel { id: platformsModel; }

        header: Rectangle {
            color: "transparent";
            height: PhxTheme.common.menuTitleHeight;
            anchors { left: parent.left; right: parent.right; }

            Text {
                text: qsTr( "Systems" );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: PhxTheme.common.menuItemMargin; }
                font { pixelSize: PhxTheme.selectionArea.headerFontSize; bold: true; }
                color: PhxTheme.selectionArea.highlightFontColor;
            }

            MouseArea {
                anchors.fill: parent;
                hoverEnabled: false;
                propagateComposedEvents: false;
                acceptedButtons: Qt.AllButtons;
            }
        }

        delegate: Item {
            property alias marqueeText: platformText;
            property int delegateIndex: index;
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            Connections {
                target: listView
                onCurrentIndexChanged:
                    if( listView.currentIndex === index ) {
                        // Always use UUID
                        if ( index === 0 ) { contentArea.contentLibraryModel.clearFilter( "games", "system" ); }
                        else { contentArea.contentLibraryModel.setFilter( "games", "system", listView.model.get( index )[0] ); }
                }
            }

            MarqueeText {
                id: platformText;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: PhxTheme.common.menuItemMargin; rightMargin: PhxTheme.common.menuItemMargin; }
                horizontalAlignment: Text.AlignLeft;
                // Print friendly name if one exists
                text: listView.model.get( index )[1] !== "" ? listView.model.get( index )[1] : listView.model.get( index )[0];
                fontSize: PhxTheme.common.baseFontSize + 1;
                color: index === listView.currentIndex ? PhxTheme.common.menuSelectedColor : PhxTheme.selectionArea.baseFontColor;
                spacing: 40;
                running: index === listView.currentIndex || mouseArea.containsMouse;
                pixelsPerFrame: 2.0;
                bold: index === listView.currentIndex;
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: {
                    if ( !contentArea.contentStackView.currentItem.objectName.localeCompare( "PlatformsView" ) )
                        contentArea.contentStackView.push( { item: contentArea.boxartGrid, replace: true } );

                    if( listView.currentIndex !== index ) listView.currentIndex = index;
                }
            }
        }
    }
}
