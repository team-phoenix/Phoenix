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
            width: 1;
            color: root.borderColor;
        }

        //Rectangle {
        //    anchors {
         //       top: parent.top;
        //        bottom: parent.bottom;
         //   }
        //    width: 1;
         //   color: "#191919";
        //}
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
                text: "Consoles";
                color: "#f1f1f1";
                font {
                    bold: true;
                    family: "Sans";
                    pixelSize: 12;
                }
            }
            Image {
                y: 2;
                source: "../assets/arrow-down-b.png";
                fillMode: Image.PreserveAspectFit;
                height: 14;
                width: 20;
                sourceSize {
                    width: 25;
                    height: 25;
                }
            }
        }
    }

    ListView {
        id: listView;
        visible: (height !== 0);
        anchors {
            top: consoleHeader.bottom;
            //bottom: parent.bottom;
            right: parent.right;
            left: parent.left;
            topMargin: 0;
        }

        height: retractList ? 0 : 500;

        Behavior on height {
            PropertyAnimation {}
        }

        snapMode: ListView.SnapToItem;
        orientation: ListView.Vertical;
        interactive: true;
        highlightFollowsCurrentItem: false;

        property bool retractList: false;

        highlight: Item {
            id: highlightItem;
            visible: !listView.retractList;
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

        model: phoenixLibrary.systemsModel();

        ExclusiveGroup {
            id: consoleGroup
        }

        delegate: Item {
            //visible: !listView.retractList;
            height: 25;
            width: consoleBar.width;
            Row {
                id: row;
                anchors {
                    fill: parent;
                    leftMargin: 25;
                }
                spacing: 7;

                Image {
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

                Text {
                    id: consoleItem;
                    anchors.verticalCenter: parent.verticalCenter;
                    width: 140;
                    text: modelData;
                    color: "#f1f1f1";
                    renderType: Text.QtRendering;
                    elide: Text.ElideRight;
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
                    if (modelData === "All") {
                        phoenixLibrary.model().setFilter("title LIKE ?", ['%%'])
                    }
                    else {
                        phoenixLibrary.model().setFilter("system = ?", [modelData]);
                    }
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
        visible: false;//consoleBar.progressText !== "";
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
