import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import vg.phoenix.themes 1.0

ScrollView {
    width: 100;
    height: 62;


    ListView {
        id: listView;
        spacing: 0;
        model: ListModel {
            id: settingsModel;
            ListElement { section: "Input Controls"; }
            //ListElement { section: "Video Playback"; }
            //ListElement { section: "Audio Playback"; }
        }

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            //width: listView.currentItem.width;
            //height: listView.currentItem.height;
            anchors.fill: listView.currentItem;

            Rectangle {
                id: highlighterRectangle;
                anchors.fill: parent;
                color: PhxTheme.common.normalButtonColor;
            }
        }

        header: Item {
            height: 36;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                text: qsTr( "Settings" );
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
        }

        delegate: Item {
            id: listViewDelegate;
            height: 25;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            Text {
                id: sectionText;
                text: section;

                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin:  24;
                }

                font {
                    pixelSize: PhxTheme.selectionArea.basePixelSize;
                }

                color: index === listView.currentIndex ? PhxTheme.common.highlighterFontColor : PhxTheme.common.baseFontColor;

            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index
                    switch ( index ) {
                    case 0:
                        contentArea.contentStackView.push( { item: contentArea.contentInputView, replace: true } );
                        break;
                    case 1:
                        break;
                    case 2:
                        break;
                    default:
                        break;
                    }
                }
            }

        }
    }
}

