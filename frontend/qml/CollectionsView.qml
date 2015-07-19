import QtQuick 2.3
import QtQuick.Controls 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

ScrollView {
    width: 100
    height: 62

    ListView {
        id: listView;
        spacing: 0;
        model: CollectionsModel {
            id: collectionsModel;
        }

        header: Rectangle {
            color: "lightyellow";
            height: 36;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                text: qsTr( "Collections" );
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin: 12;
                }

                font {
                    pixelSize: PhxTheme.sectionArea.headerFontSize;
                    bold: true;
                }

                color: PhxTheme.common.baseFontColor;
            }

            Button {
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                    rightMargin: 24;
                }

                text: qsTr( "Add" );
                onClicked: {
                    collectionsModel.append( { "collectionID": listView.count
                                                , "collectionName": "New Collection "
                                                                    + listView.count } );
                    listView.currentIndex = listView.count - 1;
                    listView.currentItem.state = "ADDED";
                    console.log( listView.currentItem.state)
                }
            }

        }

        delegate: Rectangle {
            id: listViewDelegate;
            color: index % 2 == 0 ? "orange" : "red";
            height: 24;

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

            anchors {
                left: parent.left;
                right: parent.right;
            }

            TextField {
                id: platformText;
                text: collectionName;
                readOnly: true;

                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin:  24;
                }

                onAccepted: {
                    collectionsModel.set( collectionID, platformText.text );
                    readOnly = true;
                    focus = false;
                }

                font {
                    pixelSize: PhxTheme.sectionArea.normalFontSize;
                }

                textColor: PhxTheme.common.baseFontColor;

            }

            Button {
                visible: collectionID !== 0;
                z: mouseArea.z + 1;
                anchors {
                    right: parent.right;
                    verticalCenter: parent.verticalCenter;
                    rightMargin: 12;
                }

                text: "Remove";

                onClicked: {
                    collectionsModel.remove( collectionID );
                }
            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                enabled: platformText.readOnly;
                onClicked: {

                    console.log( collectionID );

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

