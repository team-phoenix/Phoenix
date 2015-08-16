import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

ScrollView {

    ListView {
        id: listView;
        spacing: 0;
        model: PlatformsModel {
            id: platformsModel;

        }

        header: Rectangle {
            color: "transparent";
            height: 36;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                text: qsTr( "Systems" );
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin: 12;
                }

                font {
                    pixelSize: PhxTheme.selectionArea.headerFontSize;
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

                text: qsTr( "All" );
                onClicked: {
                    listView.currentIndex = -1;
                    contentArea.contentLibraryModel.clearFilter( "games", "system" );
                }

            }
        }

        delegate: Item {
            height: 26;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                id: platformText;
                text: listView.model.get( index );
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin:  24;
                }

                font {
                    pixelSize: PhxTheme.selectionArea.basePixelSize;
                    bold: true;
                }

                color: index === listView.currentIndex
                            ? PhxTheme.selectionArea.highlightFontColor : PhxTheme.selectionArea.baseFontColor;

            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index;
                    contentArea.contentLibraryModel.setFilter( "games", "system", platformText.text );
                }
            }

        }
    }
}
