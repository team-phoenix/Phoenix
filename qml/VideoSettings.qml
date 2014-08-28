import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1

Rectangle {
    id: video;
    color: settingsBubble.stackBackgroundColor;

    Column {
        anchors {
            fill: parent;
            topMargin: 25;
        }
        spacing: 25;

        Column {
            id: videoHeader;
            anchors {
                left: parent.left;
                //top: parent.top;
                //topMargin: 25;
                leftMargin: 25;
            }
            spacing: 5;

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

                Switch {
                    id: vsyncSwitch;
                    anchors.right: parent.right;
                    onCheckedChanged: {
                        if (checked)
                            root.swapInterval = 1;
                        else
                            root.swapInterval = 0;
                        console.log("You need to reset Phoenix for the change to take effect");
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
                    text: "Auto Fullscreen"
                    renderType: Text.QtRendering;
                    color: settingsBubble.alternateTextColor;
                    font {
                        family: "Sans";
                        pixelSize: 13;
                    }
                }

                Switch {
                    id: autoFullscreenSwitch;
                    anchors.right: parent.right;
                }
            }
        }

        Column {
            id: filterHeader;
            anchors {
                left: parent.left;
                leftMargin: 25;
            }
            spacing: 5;

            Text {
                text: "Filtering";
                renderType: Text.QtRendering;
                color: settingsBubble.textColor;
                font {
                    family: "Sans";
                    pixelSize: 16;
                }
            }

            Text {
                text: "Enhance the output"
                renderType: Text.QtRendering;
                color: settingsBubble.alternateTextColor;
                font {
                    family: "Sans";
                    pixelSize: 14;
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

                Switch {
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
            id: aspectRatioHeader;
            anchors {
                left: parent.left;
                leftMargin: 25;
            }
            spacing: 5;

            Text {
                text: "Aspect Settings";
                renderType: Text.QtRendering;
                color: settingsBubble.textColor;
                font {
                    family: "Sans";
                    pixelSize: 16;
                }
            }

            Text {
                text: "Adjust the video aspect ratio"
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

                Switch {
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
