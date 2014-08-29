import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1


Item {
    id: audio;

    Column {
        anchors {
            fill: parent;
            topMargin: 15;
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
                        text: "Audio Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Edit the audio output"
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
                    leftMargin: 25;
                    rightMargin: 25;
                    bottom: parent.bottom;
                    bottomMargin: 17;
                }
                height: 1;
            }
        }


        Column {
            spacing: 25;
            width: 200;
            anchors {
                left: parent.left;
                leftMargin: 25;
            }

            RowLayout {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                spacing: 25;
                Text {
                    text: "Mute"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

                PhoenixSwitch {
                    anchors.right: parent.right;
                    checked: (headerBar.volumeLevel=== 0.0);
                    property real previousLevel: headerBar.volumeLevel;
                    onCheckedChanged: {

                        if (checked) {
                            if (headerBar.volumeLevel !== 0.0) {
                                previousLevel = headerBar.volumeLevel;
                                headerBar.volumeLevel = 0.0;
                            }
                            else {
                                previousLevel = 1.0;
                            }

                        }
                        else {
                            headerBar.volumeLevel = previousLevel;
                        }

                        console.log(headerBar.volumeLevel);

                    }
                }
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

                ComboBox {
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
