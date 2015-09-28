import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0

import vg.phoenix.themes 1.0

Item {
    id: headerBar;
    width: 100
    height: 65

    property int leftPadding: 12;
    property int rightPadding: 12;
    property int topPadding: 24;
    property int bottomPadding: -36;

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.Linear; duration: 900 }
    }

    MouseArea {
        anchors.fill: parent;
        propagateComposedEvents: false;
    }

    states: [
        State {
            name: "SELECTION";
            PropertyChanges {
                target: headerBar;
                width: contentArea.width - ( rightPadding + leftPadding );
                y: contentArea.y + topPadding;
                x: contentArea.x + leftPadding;
            }
        },
        State {
            name: "PLAYING_GAME";
            PropertyChanges {
                target: headerBar;
                width: contentArea.width - ( rightPadding + leftPadding );
                x: root.width + leftPadding;
                y: contentArea.y + topPadding;
            }
        }

    ]

    DropShadow {
        anchors.fill: source;
        source: headerBarBackground;
        verticalOffset: 1;
        horizontalOffset: 0;
        color: "#a0000000";
        transparentBorder: true;
        samples: radius * 2;
        radius: 6;
    }

    OpacityMask {
        anchors.fill: headerBarBackground;
        source: headerBarSource;
        maskSource: headerBarBackground;
    }

    Item {
        id: headerBarSource;
        anchors.fill: headerBarBackground;
        visible: true;

        RowLayout {
            anchors.fill: parent;
            spacing: 0;

            Rectangle {
                color: "transparent"//"blue";
                Layout.fillHeight: true;
                width: 150;
                clip: true;

                Button {
                    anchors.centerIn: parent;
                    text: qsTr( "Delete Library" );
                    onClicked: libraryModel.clear();
                }
            }

            Rectangle {
                color: "transparent"//"darkblue";
                Layout.fillHeight: true;
                Layout.fillWidth: true;

                Row {
                    spacing: 12;
                    anchors.fill: parent;

                    Button {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        z: 100;
                        text: qsTr( "Grid" );
                        onClicked: contentAreaStackView.push( { item: boxArtGridComponent, replace: true } )
                    }

                    Button {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }

                        text: qsTr( "Details" );
                        //onClicked: contentAreaStackView.showDetailedView = !contentAreaStackView.showDetailedView;
                    }

                    /*
                    Button {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }

                        text: qsTr("Cancel Import");
                        onClicked: {
                            libraryModel.cancelInsert();
                        }
                    }

                    Button {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }

                        text: qsTr("Pause Import");
                        onClicked: {
                            if ( libraryModel.insertPaused )
                                libraryModel.resumeInsert();

                            else
                                libraryModel.pauseInsert();
                        }
                    }
                    Label {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        text: libraryModel.progress + "%";
                        color: "white";
                    }
*/
                    Label {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        text: libraryModel.message;
                        color: PhxTheme.common.baseFontColor;
                    }
                }
            }

            PhxSearchBar {
                id: searchBar;
                Layout.fillHeight: true;
                width: 50;
                backgroundColor: PhxTheme.common.secondaryBackgroundColor;
            }
        }
    }

    Rectangle {
        id: headerBarBackground
        anchors.fill: parent;
        color: "white";
        radius: 6;
        visible: false;

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                left: parent.left;
            }

            width: parent.width * ( libraryModel.progress / 100 );

            Behavior on width {
                PropertyAnimation {
                    duration: 150;
                    easing.type: Easing.Linear;
                }
            }

            color: "cyan";
            radius: parent.radius;

            Rectangle {
                color: parent.color;
                radius: 0;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    right: parent.right;
                }

                width: parent.width < parent.radius ? parent.width : parent.radius;

            }
        }
    }

}

