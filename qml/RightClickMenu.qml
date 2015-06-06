import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: rightClickMenu;
    width: 105;
    height: 200;
    color: "red";
    radius: 3;


    ListView {
        id: listView;
        height: parent.height;
        width: parent.width;
        model: ListModel {
            ListElement {title: "Play";}
           // ListElement {title: "Compress";}
            ListElement {title: "Show System";}
            ListElement {title: "Remove File";}

        }

        delegate: Item {
            height: 23;
            width: rightClickMenu.width;
            Text {
                text: title;
                color: "#f1f1f1";
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: parent.left;
                    leftMargin: 12;
                }

                font {
                    family: "Sans";
                    pixelSize: 10;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if (title === "Play") {
                        var curItem = gridView.currentItem;
                        var core = phoenixLibrary.getSystem(curItem.systemName);
                        console.log("play")
                        root.gameAndCoreCheck(curItem.titleName, curItem.systemName, curItem.fileName, core);
                    }

                    else if (title == "Remove File") {
                        phoenixLibrary.deleteRow(gridView.currentItem.titleName);
                    }
                }
            }

            Column {
                //visible: index !== listView.count - 1;
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    leftMargin: 1;
                    right: parent.right;
                }
                Rectangle {
                    color: "black";
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 1;
                }
                Rectangle {
                    color: "#404040";
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
