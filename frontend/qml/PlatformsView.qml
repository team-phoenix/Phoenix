import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

ScrollView {

    ListView {
        id: listView;
        spacing: 0;
        model: PlatformsModel {
            id: platformsModel;

        }

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            //width: listView.currentItem.width;
            //height: listView.currentItem.height;
            anchors.fill: listView.currentItem;
            anchors.rightMargin: 1;

            DropShadow {
                id: dropShadow;
                verticalOffset: 0;
                horizontalOffset: 0;
                radius: 16;
                samples: radius * 2;
                color: "red";
                transparentBorder: true;
                source: highlighterRectangle;
                anchors.fill: source;
            }

            Rectangle {
                id: highlighterRectangle;
                anchors.fill: parent;
                gradient: PhxTheme.common.primaryButtonColor;

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        right: parent.right;
                    }
                    height: 1;
                    color: "white";
                    opacity: 0.5;

                }

                Rectangle {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    height: 1;
                    opacity: 0.2;


                    RadialGradient {
                        anchors.fill: parent
                        gradient: Gradient {
                            GradientStop { position: 0.0; color: "white" }
                            GradientStop { position: 0.5; color: "black" }
                        }

                    }
                }
            }
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

                color: PhxTheme.selectionArea.highlightFontColor;
            }

            /*
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
            */
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
