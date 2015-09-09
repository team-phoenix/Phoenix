import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

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

        highlight: Item {
            x: listView.currentItem.x;
            y: listView.currentItem.y;
            //width: listView.currentItem.width;
            //height: listView.currentItem.height;
            anchors.fill: listView.currentItem;
            anchors.rightMargin: 3;

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

                color: PhxTheme.common.baseFontColor;
            }
        }

        delegate: Item {
            id: listViewDelegate;
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

