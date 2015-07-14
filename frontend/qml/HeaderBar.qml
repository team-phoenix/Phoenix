import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Dialogs 1.2

Item {
    id: headerBar;
    width: 100
    height: 62

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
        z: parent.z + 1;

        RowLayout {
            anchors.fill: parent;
            spacing: 0;

            Rectangle {
                color: "blue";
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
                color: "darkblue";
                Layout.fillHeight: true;
                Layout.fillWidth: true;

                FileDialog {
                    id: fileDialog;
                    selectFolder: true;
                    onAccepted: {
                        libraryModel.append( fileUrl );
                    }
                }

                Row {
                    spacing: 12;
                    anchors.fill: parent;
                    Button {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }

                        text: qsTr( "Add Folder" );
                        onClicked: fileDialog.open();
                    }

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

                    Label {
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        text: libraryModel.message;
                        color: "white";
                    }
                }
            }


            PhxSearchBar {
                id: searchBar;
                Layout.fillHeight: true;
                width: 42;
            }
        }

    }

    Rectangle {
        id: headerBarBackground
        anchors.fill: parent;
        color: "white";
        radius: 6;
        visible: false;
    }

        /*
        Window {
            id: dropDown;
            height: 200;
            width: 125;
            x: root.x + dock.width + ( ( headerBar.anchors.leftMargin + systemDropDown.width ) / 2 - ( width / 3));
            y: root.y + headerBar.anchors.topMargin + headerBar.height;
            flags: Qt.Popup | Qt.FramelessWindowHint;

            color: "transparent";

            Rectangle {
                anchors {
                    fill: parent;
                    topMargin: 13;
                    bottomMargin: 25;
                }
                radius: 6;

                Rectangle {
                    rotation: 45;
                    color: parent.color;
                    anchors {
                        top: parent.top;
                        topMargin: -height / 2;
                        horizontalCenter: parent.horizontalCenter;
                    }
                    height: 13;
                    width: height;
                }
            }
        }
        */

}

