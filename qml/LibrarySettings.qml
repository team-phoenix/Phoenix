import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1


Item {
    id: audio;

    Column {
        anchors {
            fill: parent;
            topMargin: 15;
            rightMargin: 15;
            leftMargin: 15;
        }
        spacing: 10;

        Item {
            height: 75;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                }
                spacing: 10;

                Image {
                    id: audioImage;
                    source: "../assets/volume-high-8x.png";
                    height: 48;
                    width: 48;
                }

                Column {
                    id: videoHeader;
                    anchors.verticalCenter: audioImage.verticalCenter;
                    spacing: 2;

                    Text {
                        text: "Library Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Modify the game library"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 13;
                        }
                    }
                }
            }

            Rectangle {
                color: "#141414";
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                    bottomMargin: 17;
                }
                height: 1;
            }
        }


        Column {
            spacing: 25;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            PhoenixTextField {
                anchors {
                    right: parent.right;
                    left: parent.left;
                }
                placeholderText: "/path/to/gamelibrary.sql";
            }

            RowLayout {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                spacing: 25;

                Text {
                    text: "Available Devices"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

                PhoenixComboBox {
                    anchors.right: parent.right;
                    implicitWidth: 100;
                    model: gameView.video.getAudioDevices();
                    onCurrentIndexChanged: {
                        gameView.video.audioDevice = currentIndex;
                    }
                }
            }
        }
    }
}
