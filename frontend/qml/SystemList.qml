import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.paths 1.0

import "../"
import "qrc:/Util"
import "qrc:/Frontend"

PhxScrollView {
    PhxListView {
        id: listView;
        anchors.fill: parent;

        currentIndex: -1;

        spacing: 0;
        model: SqlThreadedModel {
            id: sqlModel;
            fileLocation: PhxPaths.qmlMetadataLocation() + "/libretro.sqlite";
            tableName: "system";
            autoCreate: false;
            databaseSettings {
                connectionName: "LIBRETRO";
            }

            SqlColumn { name: "UUID"; type: "TEXT"; }
            SqlColumn { name: "friendlyName"; type: "TEXT"; }
            SqlColumn { name: "shortName"; type: "TEXT"; }
            SqlColumn { name: "manufacturer"; type: "TEXT"; }

            Component.onCompleted: {
                sqlModel.finishModelConstruction();
                sqlModel.setFilter( "enabled", 1, SqlModel.Exact );
            }
        }

        // Let the user select the null index as we use that for the "All" button
        // Otherwise duplicate the default behavior
        Keys.onUpPressed: {
            if( listView.currentIndex > -1 )
                listView.currentIndex--;
        }

        header: Item {
            anchors { left: parent.left; right: parent.right; }
            height: PhxTheme.common.menuTitleHeight + PhxTheme.common.menuItemHeight;

            // A simple "Systems" label
            Rectangle {
                id: headerLabel;
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

            // A fake button within the header made to look like it's the first entry in the list
            Item {
                height: PhxTheme.common.menuItemHeight;
                anchors { top: headerLabel.bottom; left: parent.left; right: parent.right; }

                Connections {
                    target: listView
                    onCurrentIndexChanged:
                        if( listView.currentIndex === -1 ) {
                            library.libraryModel.clearFilter( "system" );
                    }
                }

                MarqueeText {
                    id: allPlatformText;
                    anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: PhxTheme.common.menuItemMargin; rightMargin: PhxTheme.common.menuItemMargin; }
                    horizontalAlignment: Text.AlignLeft;
                    text: "All";
                    fontSize: PhxTheme.common.baseFontSize + 1;
                    color: listView.currentIndex === -1 ? PhxTheme.common.menuSelectedColor : PhxTheme.selectionArea.baseFontColor;
                    spacing: 40;
                    running: listView.currentIndex === -1 || mouseArea.containsMouse;
                    pixelsPerFrame: 2.0;
                    bold: listView.currentIndex === -1;
                }

                MouseArea {
                    id: mouseArea;
                    anchors.fill: parent;
                    hoverEnabled: true;

                    onClicked: if( listView.currentIndex !== -1 ) listView.currentIndex = -1;
                }

                // The highlighter from PhxListView, hard-coded for a vertical list and to only appear when index is -1
                Rectangle {
                    id: highlighter;
                    width: listView.activeFocus ? listView.growOnFocusValue : 4;
                    height: parent.height;
                    color: listView.highlighterColor;

                    visible: listView.currentIndex === -1;

                    Behavior on width { PropertyAnimation { duration: 200; } }

                    anchors.left: parent.left;

                    Connections {
                        target: listView;
                        onActiveFocusChanged:
                            showAnimation.complete();
                        onCurrentIndexChanged: {
                            if( listView.currentIndex === -1 ) {
                                showAnimation.complete();
                                showAnimation.start();
                            }
                        }
                    }

                    SequentialAnimation {
                        id: showAnimation;
                        PropertyAction { target: highlighter; property: "y"; value: 0; }
                        PropertyAnimation {
                            target: highlighter;
                            property: "width";
                            from: 1;
                            to: listView.activeFocus ? listView.growOnFocusValue : 4;
                            duration: 300;
                            easing.type: Easing.InOutQuart;
                        }
                    }
                }
            }
        }

        delegate: Item {
            property alias marqueeText: platformText;
            property int delegateIndex: index;
            height: PhxTheme.common.menuItemHeight;
            anchors { left: parent.left; right: parent.right; }

            // Change the libraryModel's (Library.qml) system filter to the UUID of the currently selected system name
            Connections {
                target: listView
                onCurrentIndexChanged:
                    if( listView.currentIndex === index ) {
                        library.libraryModel.setFilter( "system", UUID );
                }
            }

            MarqueeText {
                id: platformText;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: PhxTheme.common.menuItemMargin; rightMargin: PhxTheme.common.menuItemMargin; }
                horizontalAlignment: Text.AlignLeft;
                // Print friendly name if one exists
                text: ( !friendlyName || friendlyName.length === 0 ) ? UUID : manufacturer + " - " + friendlyName;
                fontSize: PhxTheme.common.baseFontSize + 1;
                color: index === listView.currentIndex ? PhxTheme.common.menuSelectedColor : PhxTheme.selectionArea.baseFontColor;
                spacing: 40;
                running: index === listView.currentIndex || delegateMouseArea.containsMouse;
                pixelsPerFrame: 2.0;
                bold: index === listView.currentIndex;
            }

            MouseArea {
                id: delegateMouseArea;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: if( listView.currentIndex !== index ) listView.currentIndex = index;
            }
        }
    }
}
