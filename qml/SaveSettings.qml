import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    color: settingsBubble.contentColor;

    Column {
        anchors {
            topMargin: 15;
            fill: parent;
        }
        spacing: 20;
        Text {
            text: "Dropbox";
            color: settingsBubble.textColor;
            renderType: Text.QtRendering;
            anchors {
                horizontalCenter: parent.horizontalCenter;
            }
            font {
                family: "Sans";
                pixelSize: 16;
                bold: true;
            }
        }

        Rectangle {
            color: settingsBubble.groupingColor;
            height: 125;
            width: parent.width * 0.85;
            anchors.horizontalCenter: parent.horizontalCenter;

            Column {
                anchors.fill: parent;
                anchors.topMargin: 10;
                anchors.leftMargin: 25;
                spacing: 5;
                Row {
                    spacing: 57;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 50;

                    Label {
                        text: "Login: ";
                        anchors.verticalCenter: parent.verticalCenter;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                            bold: true;
                        }
                    }

                    TextField {
                        anchors.verticalCenter: parent.verticalCenter;
                        width: parent.width * 0.5;
                        height: 25;
                    }
                }

                Row {
                    spacing: 25;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 50;

                    Label {
                        text: "Password: ";
                        anchors.verticalCenter: parent.verticalCenter;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                            bold: true;
                        }
                    }

                    TextField {
                        width: parent.width * 0.5;
                        anchors.verticalCenter: parent.verticalCenter;
                        height: 25;
                        textColor: settingsBubble.textColor;
                    }
                }
            }
        }

        Label {
            text: "Save Location";
            color: settingsBubble.textColor;
            anchors.horizontalCenter: parent.horizontalCenter;
            font {
                family: "Sans";
                pixelSize: 16;
                bold: true;
            }
        }

        Rectangle {
            width: parent.width * 0.85;
            anchors.horizontalCenter: parent.horizontalCenter;

            color: settingsBubble.groupingColor;
            height: 75;

            TextField {
                id: savePathField;
                text: "";
                anchors {
                    fill: parent;
                    leftMargin: 25;
                    rightMargin: 25;
                    topMargin: 24;
                    bottomMargin: 24;
                }
            }
        }

        Text {
            text: "Save States";
            renderType: Text.QtRendering;
            color: settingsBubble.textColor;
            anchors.horizontalCenter: parent.horizontalCenter;
            font {
                family: "Sans";
                bold: true;
                pixelSize: 16;
            }
        }

        Rectangle {
            height: 125;
            width: parent.width * 0.85;
            anchors.horizontalCenter: parent.horizontalCenter;
            color: settingsBubble.groupingColor;

            Column {
                anchors.fill: parent;

                Row {
                    height: 50;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }

                    Label {
                        text: "Location: ";
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            bold: true;
                            pixelSize: 14;
                        }
                    }
                    TextField {
                        width: parent.width * 0.5;
                        textColor: settingsBubble.textColor;
                    }
                }
            }
        }
    }
}
