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
            color: "lightblue";
            height: 36;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                text: qsTr( "SYSTEMS" );
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

                text: qsTr( "All" );
                onClicked: contentArea.contentLibraryModel.clearFilter( "games", "system" );

            }
        }

        delegate: Rectangle {
            color: index % 2 == 0 ? "yellow" : "lightgreen";
            height: 24;

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
                    pixelSize: PhxTheme.sectionArea.normalFontSize;
                }

                color: PhxTheme.common.baseFontColor;

            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    contentArea.contentLibraryModel.setFilter( "games", "system", platformText.text );
                }
            }

        }
    }
}
