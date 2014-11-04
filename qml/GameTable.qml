import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TableView {
    id: table;

    property string itemName: "table";
    property string highlightColor: "#4a4a4a";
    property string textColor: "#f1f1f1";
    property string headerColor: "#4f474e";

    model: phoenixLibrary.model();

    TableViewColumn {role: "title" ; title: "Name"; width: 350;}
    TableViewColumn {role: "system"; title: "Console"; width: 200; }
    TableViewColumn {role: "time_played"; title: "Time Played"; width: 125; }
    TableViewColumn {role: "filename"; title: "File Name"; width: 200; }


    onSortIndicatorColumnChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)
    onSortIndicatorOrderChanged: model.sort(sortIndicatorColumn, sortIndicatorOrder)

    ExclusiveGroup {
        id: rowGroup;
    }

    Component.onCompleted: {
        root.itemInView = "table";
    }

    headerDelegate: Rectangle {
        color: table.headerColor;
        height: 25;
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
                pixelSize: 11;
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
        property int handleWidth: 10
        /*frame: Rectangle {
            color: gameGrid.color;
            //width: 0;
            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#1a1a1a";
            }
        }*/

        transientScrollBars: true;
        scrollToClickedPosition: true;
        handleOverlap: -3;

        handle: Rectangle {
            color: "red"
            radius: 3;
            x: 3;
            implicitWidth: tableStyle.handleWidth - (x * 2);
            implicitHeight: 30;
            gradient: Gradient {
                GradientStop {position: 0.0; color: "#2b2a2b";}
                GradientStop {position: 1.0; color: "#252525";}
            }

            Rectangle {
                color: "#525254";
                anchors {
                    top: parent.top;
                    left: parent.left;
                    leftMargin: 1;
                    rightMargin: 1;
                    right: parent.right;
                }
                height: 1;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    topMargin: 1;
                }
                color: "#414142";
                width: 1;
            }

            Rectangle {
                anchors {
                    right: parent.right;
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    topMargin: 1;
                }
                color: "#414142";
                width: 1;
            }

            Rectangle {
                color: "#323233";
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    leftMargin: 1;
                    rightMargin: 1;
                    right: parent.right;
                }
                height: 1;
            }
        }

        scrollBarBackground: Rectangle {
            radius: 3;
            color: "#1c1c1c";
            width: styleData.hovered ? 16 : 12;
            onWidthChanged:  {
                if (styleData.hovered)
                    tableStyle.handleWidth = 16;
                else
                    tableStyle.handleWidth = 12;
            }

            border {
                width: 1;
                color: "#1a1a1a";
            }
        }

        rowDelegate: Rectangle {
            id: row;
            color: styleData.alternate ? "#262626" : "#2e2e2e";
            width: parent.width;
            height: 25;

            property ExclusiveGroup exclusiveGroup: rowGroup;
            property bool checked: false;

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
                onDoubleClicked: {
                    row.checked = true;
                    var system_name = table.model.get(styleData.row)["system"];
                    var core = phoenixLibrary.getSystem(system_name);
                    var file_name = table.model.get(styleData.row)["filename"];
                    if (core !== "" && file_name !== "")
                        windowStack.push({item: gameView, properties: {coreName: core, gameName: file_name, run: true}});
                }

                onClicked: {
                    row.checked = true;
                }
            }
        }

        itemDelegate: Item {
            width: control.width;
            height: 25;

            Text {
                anchors {
                    left: parent.left;
                    leftMargin: 15;
                    verticalCenter: parent.verticalCenter;
                }
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
