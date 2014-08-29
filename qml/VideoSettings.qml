import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1

Rectangle {
    id: video;
    gradient: Gradient {
        GradientStop {position: 0.0; color: "#323232";}
        GradientStop {position: 1.0; color: "#272727";}

    }

    border {
        width: 1;
        color: "#0b0b0b";
    }

    Rectangle {
        anchors {
            top: parent.top;
            topMargin: video.border.width;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "#4d4d4d";
    }

    Rectangle {
        anchors {
            left: parent.left;
            leftMargin: video.border.width;
            top: parent.top;
            bottom: parent.bottom;
        }
        color: "#383838";
        width: 1
    }

    Rectangle {
        anchors {
            right: parent.right;
            rightMargin: video.border.width;
            top: parent.top;
            bottom: parent.bottom;
        }
        color: "#383838";
        width: 1
    }

    Rectangle {
        anchors {
            bottom: parent.bottom;
            bottomMargin: video.border.width;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "#2b2b2b";
    }

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
