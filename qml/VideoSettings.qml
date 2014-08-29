import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1

Item {
    id: video;

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
                    id: videoImage;
                    source: "../assets/Video-32.png";
                    height: 48;
                    width: 48;
                }

                Column {
                    id: videoHeader;
                    anchors.verticalCenter: videoImage.verticalCenter;
                    spacing: 2;

                    Text {
                        text: "Video Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Edit the video output"
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

            RowLayout {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                spacing: 25;
                Text {
                    text: "VSync"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

                PhoenixSwitch{
                    anchors.right: parent.right;
                }
            }

            RowLayout {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                spacing: 25;

                Text {
                    text: "Auto Fullscreen"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

               PhoenixSwitch {
                    id: autoFullscreenSwitch;
                    anchors.right: parent.right;
                }
            }

            RowLayout {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                spacing: 25;
                Text {
                    text: "Smooth"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 14;
                    }
                }

                PhoenixSwitch {
                    id: filteringSwitch;
                    anchors.right: parent.right;
                    checked: (gameView.filtering === 2);
                    onCheckedChanged: {
                        if (checked)
                            gameView.filtering = 2;
                        else
                            gameView.filtering = 1;
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
                    text: "Stretch Video"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 14;
                    }
                }

                PhoenixSwitch {
                    id: ratioSwitch;
                    anchors.right: parent.right;
                    checked: gameView.stretchVideo;
                    onCheckedChanged: {
                        if (checked)
                            gameView.stretchVideo = true;
                        else
                            gameView.stretchVideo = false;

                    }

                }
            }
        }
    }
}
