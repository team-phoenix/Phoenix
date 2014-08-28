import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0

Rectangle {
    id: consoleBar;
    color: "#2b2b2b";
    height: 500;
    width: 250;

    property real progressValue: gamelibrary.progress;
    property string progressText: gamelibrary.label;

    Rectangle {
        id: topBord;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "#1a1a1a";
    }

    Rectangle {
        id: rightBord;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            right: parent.right;
        }
        width: 1;
        color: "#1a1a1a";
    }

    Row {
        id: leftBord;
        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 2;
            color: root.borderColor;
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#191919";
        }
    }

    ListView {
        id: listView;
        anchors {
            top: parent.top;
            topMargin: 15;
        }

        height: parent.height / 2;
        snapMode: ListView.SnapToItem;
        orientation: ListView.Vertical;
        interactive: true;
        spacing: 5;
        highlightFollowsCurrentItem: false;

        header: Item {
            height: 25;
            width: parent.width;
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: 25;
                    horizontalCenter: parent.horizontalCenter;
                }
                renderType: Text.QtRendering;
                text: "Consoles";
                color: "#f1f1f1";
                font {
                    bold: true;
                    family: "Sans";
                    pixelSize: 12;
                }
            }
        }

        highlight: Item {
            id: highlightItem;
            height: listView.currentItem.height;
            width: listView.width;
            anchors.verticalCenter: listView.currentItem.verticalCenter;
            y: listView.currentItem.y;
            Item {
                id: innerItem;
                height: parent.height;
                width: parent.width;




                Rectangle {
                    id: mainColor;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top: parent.top;
                        bottom: parent.bottom;
                    }
                    color: listView.currentItem ? "#171717" : "#000000FF";
                    Rectangle {
                        id: topBorder;
                        color: "#f27b77";
                        anchors {
                            left: parent.left;
                            leftMargin: leftBord.width;
                            top: parent.top;
                        }
                        height: 2;
                        width: 4;
                    }

                    Row {
                        // leftAccent;
                        anchors {
                            left: parent.left;
                            leftMargin: leftBord.width;
                            bottom: bottomB.top;
                            top: topBorder.bottom;
                        }

                        Rectangle {
                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                            width: 1;
                            color: "#db5753";
                        }

                        Rectangle {
                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                            width: 3;
                            color: "#e8433f";
                        }

                    }

                    Column {
                        id: bottomB;
                        anchors {
                            right: parent.right;
                            rightMargin: rightBord.width;
                            left: parent.left;
                            leftMargin: leftBord.width;
                            bottom: parent.bottom;
                        }

                        Rectangle {
                            color: "#a22f2c";
                            anchors {
                                left: parent.left;
                            }
                            width: 4;
                            height: 2;
                        }
                        Rectangle {
                            color: "#474747";
                            anchors {
                                left: parent.left;
                                right: parent.right;
                            }
                            height: 1;
                        }
                    }
                }


            }
        }

        anchors {
            right: parent.right;
            left: parent.left;
            top: consoleLabel.bottom;
            topMargin: 10;
        }

        model: ListModel {
            ListElement {title: "All"; icon: "";}
            ListElement {title: "Nintendo"; icon: "/assets/consoleicons/nes.png";}
            ListElement {title: "Super Nintendo"; icon: "/assets/consoleicons/snes.png";}
            ListElement {title: "Nintendo 64"; icon: "/assets/consoleicons/mupen64plus.png";}
            ListElement {title: "FFmpeg"; icon: "/assets/consoleicons/ffmpeg.png";}
            ListElement {title: "Sony PlayStation"; icon: "/assets/consoleicons/ps1.png";}
            ListElement {title: "Game Boy Advance"; icon: "/assets/consoleicons/gba.png";}
            ListElement {title: "Game Boy Color"; icon: "/assets/consoleicons/gbcolor.png";}
            ListElement {title: "Nintendo DS"; icon: "/assets/consoleicons/nds.png";}
            ListElement {title: "DOSBox"; icon: "/assets/consoleicons/dosbox.png";}
            ListElement {title: "Dinothawr"; icon: "/assets/consoleicons/dinothawr.png";}
        }

        ExclusiveGroup {
            id: consoleGroup
        }

        delegate: Item {
            height: 33;
            width: consoleBar.width;
            Row {
                id: row;
                anchors.fill: parent;
                anchors.leftMargin: 45;
                spacing: 10;

                Image {
                    anchors.verticalCenter: parent.verticalCenter;
                    source: icon;
                    sourceSize {
                        height: 20;
                        width: 20;
                    }
                    height: 20;
                    width: 20;
                }

                Text {
                    id: consoleItem;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: title;
                    color: "#f1f1f1";
                    renderType: Text.QtRendering;
                    font {
                        family: "Sans";
                        pixelSize: 11;
                    }
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index;
                }
            }
        }
    }

    Text {
        id: favorites;
        z: listView.z + 1;
        //text: "Favorites";
        renderType: Text.QtRendering;

        font {
            bold: true;
            pixelSize: 12;
            family: "Sans";
        }

        color: "#f1f1f1";
        anchors {
            left: parent.left;
            leftMargin: 25;
            top: listView.bottom;
            topMargin: 40;
        }
    }

    Rectangle {
        id: progressArea;
        z: 1;
        visible: true;//consoleBar.progressText !== "";
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            leftMargin: leftBord.width;
            right: parent.right;
            rightMargin: rightBord.width;
        }

        height: 75;
        color: "#2e2e2e";

        Column {
            id: _topBord;
            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: root.borderColor;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#474747";
            }
        }

        Rectangle {
            color: "#383838";
            anchors {
                right: parent.right;
                top: _topBord.bottom;
                bottom: parent.bottom;
            }
            width: 1;
        }

        Rectangle {
            color: "#383838";
            anchors {
                left: parent.left;
                top: _topBord.bottom;
                bottom: parent.bottom;
            }
            width: 1;
        }

        Column {
            id: bottomBord;
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }

            Rectangle {
                id: bottomBorder1;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#404040";
            }

            Rectangle {
                id: bottomBorder2;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#1a1a1a";
            }

            Rectangle {
                id: bottomBorder3;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: root.borderColor;
            }
        }

        Column {
            spacing: 4;
            anchors {
                verticalCenterOffset: 0;
                centerIn: parent;
            }
            //anchors {
               // horizontalCenter: parent.horizontalCenter;
                //top: parent.top;
                //topMargin: 15;
            //}
            Label {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: consoleBar.progressText;
                font {
                    bold: true;
                    pixelSize: 14;
                }

                color: "#f1f1f1";
            }

            ProgressBar {
                id: progressBar;
                anchors.horizontalCenter: parent.horizontalCenter;
                maximumValue: 100;
                minimumValue: 0;
                value: consoleBar.progressValue;
                onValueChanged: {
                    if (value === 100)
                        consoleBar.progressText = "";
                }

                style: ProgressBarStyle {
                    background: Rectangle {
                        color: "#0b0b0b";
                        implicitWidth: 175;
                        implicitHeight: 4;
                    }
                    progress: Rectangle {
                        color: root.accentColor;
                    }
                }
            }
        }
    }
}
