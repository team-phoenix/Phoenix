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

    // The default of 20 just isn't fast enough
    __wheelAreaScrollSpeed: 100;

    ListView {
        id: listView;
        spacing: 0;
        model: PlatformsModel { id: platformsModel; }
        boundsBehavior: Flickable.StopAtBounds;
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
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            // Image {
            //     anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: PhxTheme.common.menuItemMargin; }
            //     smooth: false;
            //     sourceSize { height: height; width: width; }
            //     source: "systems/" + listView.model.get( index ) + ".svg";
            // }

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
                    if ( contentArea.contentStackView.currentItem.objectName !== "PlatformsView" ) {
                        contentArea.contentStackView.push( { item: contentArea.boxartGrid, replace: true } );
                    }

                    // Always use UUID
                    if( listView.currentIndex !== index ) {
                        listView.currentIndex = index;
                        if ( index === 0 ) { contentArea.contentLibraryModel.clearFilter( "games", "system" ); }
                        else { contentArea.contentLibraryModel.setFilter( "games", "system", listView.model.get( index )[0] ); }
                        listView.doShowAnimation();
                    }
                }
            }
        }
    }
}
