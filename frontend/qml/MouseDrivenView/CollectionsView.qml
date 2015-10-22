import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

// @disable-check M300
PhxScrollView {
    width: 100
    height: 65

    ListView {
        id: listView;
        spacing: 0;
        model: CollectionsModel { id: collectionsModel; }

        boundsBehavior: Flickable.StopAtBounds;

        header: Rectangle {
            anchors { left: parent.left; right: parent.right; }
            color: "transparent";
            height: PhxTheme.common.menuTitleHeight;

            Text {
                text: qsTr( "Collections" );
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: PhxTheme.common.menuItemMargin; }
                font { pixelSize: PhxTheme.selectionArea.headerFontSize; }
                color: PhxTheme.selectionArea.highlightFontColor;
            }

            Image {
                anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: PhxTheme.common.menuItemMargin; }
                height: 20;
                width: height;
                sourceSize { height: height; width: width; }
                source:  "add2.svg";
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        collectionsModel.append( { "collectionID": listView.count
                                                    , "collectionName": "New Collection "
                                                                        + listView.count } );
                        listView.currentIndex = listView.count - 1;
                        listView.currentItem.state = "ADDED";
                    }
                }
            }
        }

        delegate: Rectangle {
            id: listViewDelegate;
            anchors { left: parent.left; right: parent.right; }
            color: "transparent";
            height: PhxTheme.common.menuItemHeight;

            states: [
                State {
                    name: "ADDED;";
                    PropertyChanges {
                        target: platformText;
                        readOnly: false;
                    }
                },

                State {
                    name: "SAVED";
                    PropertyChanges {
                        target: platformText;
                        readOnly: true;
                        focus: false;
                    }
                }
            ]

            onStateChanged: {
                if ( state === "ADDED" ) {
                    platformText.selectAll();
                    platformText.forceActiveFocus();
                }
            }

            TextField {
                id: platformText;
                text: collectionName;
                readOnly: true;
                anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: PhxTheme.common.menuItemMargin; }
                style: TextFieldStyle {
                    font { pixelSize: PhxTheme.common.baseFontSize; }
                    textColor: PhxTheme.common.baseFontColor;
                    background: Rectangle {
                        color: "transparent";
                        border.width: 0;
                    }
                }

                onAccepted: {
                    collectionsModel.set( collectionID, platformText.text );
                    readOnly = true;
                    focus = false;
                }
            }

            Button {
                visible: collectionID !== 0;
                z: mouseArea.z + 1;
                anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: PhxTheme.common.menuItemMargin; }
                text: "Remove";
                onClicked: { collectionsModel.remove( collectionID ); }
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                enabled: platformText.readOnly;
                onClicked: {

                    if ( collectionID == 0 ) {
                        contentArea.contentLibraryModel.clearFilter( "collections", "collectionID" );
                    } else {
                        contentArea.contentLibraryModel.setFilter( "collections", "collectionID", collectionID );
                    }
                }
            }
        }
    }
}
