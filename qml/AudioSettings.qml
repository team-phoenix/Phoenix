import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Item {

    Column {
        anchors {
            fill: parent;
            topMargin: 25;
        }
        spacing: 25;

        Column {
            anchors {
                left: parent.left;
                //top: parent.top;
                //topMargin: 25;
                leftMargin: 25;
            }
            spacing: 5;

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
                text: "Edit the audio feed"
                renderType: Text.QtRendering;
                color: settingsBubble.alternateTextColor;
                font {
                    family: "Sans";
                    pixelSize: 13;
                }
            }
        }

        Column {
            spacing: 25;
            width: 250;
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
                    color: settingsBubble.textColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

                Switch {
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
                    color: settingsBubble.textColor;
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
