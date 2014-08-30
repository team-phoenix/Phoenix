import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

TableView {
    id: table;

    property string itemName: "table";
    property string highlightColor: "#4a4a4a";
    property string textColor: "#f1f1f1";
    property string headerColor: "#4f474e";

    model: gamelibrary;

    TableViewColumn {role: "title" ; title: "Name"; width: 350;}
    TableViewColumn {role: "console"; title: "Console"; width: 200; }
    TableViewColumn {role: "timePlayed"; title: "Time Played"; width: 200; }

    onSortIndicatorColumnChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)
    onSortIndicatorOrderChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)

    ExclusiveGroup {
        id: rowGroup;
    }

    headerDelegate: Rectangle {
        color: table.headerColor;
        height: 35;
        width: parent.width;

        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: 1;

            color: "#303030";
        }

        Row {
            id: rightRowBorder;
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
            }

            Rectangle {
                color: "#1a1a1a";
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 1;
            }
        }

        Column {
            anchors {
                right: rightRowBorder.left;
                left: parent.left;
                bottom: parent.bottom;
            }

            Rectangle {
                anchors {
                    right: parent.right;
                    left: parent.left;
                }
                color: "#1a1a1a";
                height: 1;
            }
        }



        Text {
            text: styleData.value;
            renderType: Text.QtRendering;
            anchors {
                left: parent.left;
                leftMargin: 15;
                verticalCenter: parent.verticalCenter;
            }
            color: table.textColor;

            font {
                bold: true;
                pixelSize: 12;
                family: "Sans";
            }

        }

    }

    style: TableViewStyle {
        id: tableStyle;

        frame: Rectangle {
            color: "#202020";
            width: 1;
        }
        transientScrollBars: true;
        scrollToClickedPosition: true;

        scrollBarBackground: Rectangle {
            color: "#1f1f1f";
            //height: control.height;
            width: styleData.hovered ? 17 : 15;
        }

        rowDelegate: Rectangle {
            id: row;
            color: styleData.alternate ? "#262626" : "#2e2e2e";
            width: parent.width;
            height: 32;

            property ExclusiveGroup exclusiveGroup: rowGroup;
            property bool checked: false

            onExclusiveGroupChanged: {
                if (exclusiveGroup) {
                    exclusiveGroup.bindCheckable(row);
                }
            }

            onCheckedChanged: {
                if (checked) {
                    color = table.highlightColor;
                }
                else {
                    color = styleData.alternate ? "#262626" : "#2e2e2e";
                }
            }

            MouseArea {
                anchors.fill: parent;
                onPressed: row.checked = true;
            }

        }

        itemDelegate: Item {
            Text {
                anchors {
                    left: parent.left;
                    leftMargin: 15;
                    verticalCenter: parent.verticalCenter;
                }
                renderType: Text.QtRendering;
                width: control.width;
                color: "#f1f1f1";
                text: styleData.value;

                font {
                    pixelSize: 11;
                    family: "Sans";
                }

                elide: Text.ElideRight;

            }
        }
    }
}
