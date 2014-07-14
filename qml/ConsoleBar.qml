import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    id: consoleBar;
    color: "#2b2b2b";
    height: 500;
    width: 250;

    Rectangle {
        id: consoleLabel;
        z: progressBar.z - 1;
        anchors {
            left: parent.left;
            top: parent.top;
            leftMargin: 25;
            topMargin: 25;
        }
        height: 20;
        width: 70;
        opacity: 0.8;
        //color: "#2e2e2e";
        color: "#000000FF";

        Label {

            anchors.centerIn: parent;
            text: "Consoles";
            font {
                bold: true;
                pixelSize: 16;
                family: "Sans";
            }

            color: "#f1f1f1";
        }
    }

    ListView {
        id: listView;
        z: progressBar.z - 1;
        height: parent.height / 2;
        snapMode: ListView.SnapToItem;
        orientation: ListView.Vertical;
        interactive: false;
        highlightFollowsCurrentItem: false;

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

        delegate: Button {
            id: consoleButton;
            checkable: true;
            checked: false;
            exclusiveGroup: consoleGroup;
            height: 35;
            anchors {
                left: parent.left;
            }

            width: consoleBar.width;

            style: ButtonStyle {
                background: Rectangle {
                    visible: consoleButton.checked;
                    color: "#525252";

                    Rectangle {
                        visible: parent.visible;
                        color: "#b85353";
                        anchors {
                            left: parent.left;
                            top: parent.top;
                            bottom: parent.bottom;
                        }
                        width: 15;

                    }

                    Image {
                        anchors.verticalCenter: parent.verticalCenter;
                        visible: parent.visible;
                        source: "/assets/triangle.png";
                        x: consoleButton.width;   // This makes it extend past the bar.
                        height: 35;
                        width: 35;

                        sourceSize {
                            height: 32
                            width: 43;
                        }
                    }

                }
                label: Row {
                    spacing: 10;
                    anchors {
                        left: parent.left;
                        leftMargin: 50;
                    }
                    Image {
                        anchors.verticalCenter: parent.verticalCenter;
                        source: icon;
                        sourceSize {
                            height: 20;
                            width: 20;
                        }
                    }

                    Label {
                        anchors.verticalCenter: parent.verticalCenter;
                        text: title;
                        color: "#f1f1f1";

                        font {
                            bold: true;
                            pixelSize: 14;
                            family: "Sans";
                        }

                    }
                }
                    /*Image {
                        id: triangleImg;
                        visible: control.pressed;
                        anchors {
                            right: parent.right;
                            verticalCenter: parent.verticalCenter;
                        }
                        source: "/assets/arrow-down-dark.png";
                        sourceSize {
                            width: 30;
                            height: 30;
                        }
                    }*/
            }
        }
    }

    Label {
        id: favorites;
        text: "Favorites";

        font {
            bold: true;
            pixelSize: 16;
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
        id: progressBar;
        z: 1;
        visible: true;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }

        height: 75;
        color: "#0f0f0f";

        property string showText: "Importing Games";
        onShowTextChanged: {
            if (showText !== "")
                visible = true;
        }

        Column {
            spacing: 4;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                top: parent.top;
                topMargin: 15;
            }
            Label {
                anchors.horizontalCenter: parent.horizontalCenter;
                text: progressBar.showText;
                font.bold: true;
                color: "#f1f1f1";
            }

            ProgressBar {
                anchors.horizontalCenter: parent.horizontalCenter;
                maximumValue: 100;
                minimumValue: 0;
                value: 50;
                style: ProgressBarStyle {
                    background: Rectangle {
                        color: "#878787";
                        opacity: 0.8;
                        implicitWidth: 200;
                        implicitHeight: 8;
                    }
                    progress: Rectangle {
                        color: "#b85353";
                    }
                }
            }
        }
    }
}
