import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

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
            ListElement { section: "Video Playback"; }
            ListElement { section: "Audio Playback"; }
        }

        header: Rectangle {
            color: "lightyellow";
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
                    pixelSize: PhxTheme.sectionArea.headerFontSize;
                    bold: true;
                }

                color: PhxTheme.common.baseFontColor;
            }
        }

        delegate: Rectangle {
            id: listViewDelegate;
            color: index % 2 == 0 ? "cyan" : "yellow";
            height: 24;

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
                    pixelSize: PhxTheme.sectionArea.normalFontSize;
                }

                color: PhxTheme.common.baseFontColor;

            }

            MouseArea {
                id: mouseArea;
                anchors.fill: parent;
                onClicked: {
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

