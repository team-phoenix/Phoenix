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

                }
            }

        }

        delegate: Rectangle {
            color: index % 2 == 0 ? "orange" : "red";
            height: 24;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                id: platformText;
                text: collectionName;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin:  24;
                }

                font {
                    pixelSize: PhxTheme.sectionArea.normalFontSize;
                }

                color: PhxTheme.common.baseFontColor;

            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    //contentArea.contentLibraryModel.setFilter( LibraryModel.SystemRole, "= ?", platformText.text );
                }
            }

        }
    }
}

