import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0

Rectangle {
    id: consoleBar;
    color: "#2b2b2b";
    height: 500;
    width: 250;

    property real progressValue: phoenixLibrary.progress;
    property string progressText: phoenixLibrary.label;

    property alias list: listView;

    Row {
        id: rightBord;
        anchors {
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;

        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#333131";
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 1;
            color: "#0b0b0b";
        }
    }

    Rectangle {
        id: bottomBorder;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }
        color: "#0b0b0b";
        height: 1;
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
            width: 1;
            color: "#0b0b0b";
        }

        Rectangle {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                bottomMargin: 1;
            }
            width: 1;
            color: "#333131";
        }
    }

    Rectangle {
        id: consoleHeader;
        height: 36;
        color: parent.color;
        z: listView.z + 1;
        anchors {
            top: parent.top;
            //topMargin: 12;
            left: parent.left;
            right: parent.right;
            rightMargin: 1;
        }

        Row {
            // leftBord;
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
                width: 1;
                color: "#0b0b0b";
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 1;
                }
                width: 1;
                color: "#333131";
            }
        }

        Rectangle {
            // rightBorder;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                right: parent.right;
            }
            width: 1;
            color: "#333131";
        }

        MouseArea {
            id: mouse;
            anchors.fill: parent;
            onClicked: {
                if (listView.retractList)
                    listView.retractList = false;
                else
                    listView.retractList = true;
            }

        }



        Row {
            anchors {
                left: parent.left;
                top: parent.top;
                topMargin: 12;
                leftMargin: 12;
                horizontalCenter: parent.horizontalCenter;
            }


            Text {
                renderType: Text.QtRendering;
                text: "CONSOLES";
                color: "#68686b";
                font {
                    bold: true;
                    family: "Sans";
                    pixelSize: 11
                }
            }
            /*Image {
                y: 2;
                source: "../assets/arrow-down-b.png";
                fillMode: Image.PreserveAspectFit;
                height: 14;
                width: 20;
                sourceSize {
                    width: 25;
                    height: 25;
                }
            }*/
        }
    }

    ListView {
        id: listView;
        visible: (height !== 0);
        focus: root.consoleBarFocus;
        anchors {
            top: consoleHeader.bottom;
            topMargin: 1;
            //bottom: parent.bottom;
            right: parent.right;
            left: parent.left;
        }

        height: retractList ? 0 : 500;

        Behavior on height {
            PropertyAnimation {}
        }

        keyNavigationWraps: true;
        snapMode: ListView.SnapToItem;
        orientation: ListView.Vertical;
        interactive: false;
        highlightFollowsCurrentItem: true;

        property bool retractList: false;

        Keys.forwardTo: headerBar.textField;
        Keys.priority: Keys.BeforeItem;
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Right:
                //case Qt.Key_Tab:
                    root.keyBoardFocus = 2;
                    event.accepted = true;
                    break;
                case Qt.Key_Down:
                    listView.incrementCurrentIndex();
                    event.accepted = true;
                    break;
                case Qt.Key_Up:
                    listView.decrementCurrentIndex();
                    event.accepted = true;
                    break;

                default:
                    break;
            }
        }

        highlight: Rectangle {
            id: highlightItem;
            visible: !listView.retractList;
            height: listView.currentItem.height;
            width: listView.width;
            anchors.verticalCenter: listView.currentItem.verticalCenter;
            y: listView.currentItem.y;
            gradient: Gradient {
                GradientStop {position: 0.0; color: "#0b0b0b";}
                GradientStop {position: 0.05; color: "#1a1a1a";}
                GradientStop {position: 0.95; color: "#1a1a1a";}
                GradientStop {position: 1.0; color: "#0b0b0b";}
            }

            CustomBorder {
                color: "#393737";
                shareMargin: false;
                _topMargin: 1;
                _bottomMargin: 1;
                _leftMargin: -1;
                _rightMargin: -1;
            }

            Rectangle {
                anchors {
                    right: parent.right;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "#0b0b0b"

            }

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                    topMargin: 2;
                    leftMargin: 2;
                    bottomMargin: 2;

                }
                width: 4;
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#f06612";}
                    GradientStop {position: 1.0; color: "#dc113b";}
                }

                CustomBorder {
                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "#ff944a";}
                        GradientStop {position: 1.0; color: "#ef516c";}
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

        model: phoenixLibrary.systemsModel();

        onCurrentIndexChanged: {
            if (currentItem.text === "All") {
                phoenixLibrary.model().setFilter("title LIKE ?", ['%%'])
            }
            else {
                phoenixLibrary.model().setFilter("system = ?", [currentItem.text]);
            }
        }

        ExclusiveGroup {
            id: consoleGroup
        }



        delegate: Item {
            id: item;
            //visible: !listView.retractList;
            height: 26;
            width: consoleBar.width;
            property string text: modelData;
            Row {
                id: row;
                anchors {
                    fill: parent;
                    leftMargin: 25;
                }
                spacing: 8;

                Image {
                    visible: false;
                    anchors.verticalCenter: parent.verticalCenter;
                    source: phoenixLibrary.systemIcon(modelData);
                    fillMode: Image.PreserveAspectFit;
                    sourceSize {
                        height: 24;
                        width: 24;
                    }
                    height: 20;
                    width: 20;
                }

                TextGradient {
                    id: consoleItem;
                    anchors.verticalCenter: parent.verticalCenter;
                    width: item.width * 0.6;
                    height: 22//item.height;
                    text: modelData;
                    textColor: "#f1f1f1";
                    enableGradient: index === listView.currentIndex;
                    elide: Text.ElideRight;
                    bold: index == listView.currentIndex//enableGradient;
                    pointSize: 8;

                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index;
                    //if (modelData === "All") {
                    //    phoenixLibrary.model().setFilter("title LIKE ?", ['%%'])
                    //}
                    //else {
                    //    phoenixLibrary.model().setFilter("system = ?", [modelData]);
                    //}
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
        visible: consoleBar.progressText !== "";
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
