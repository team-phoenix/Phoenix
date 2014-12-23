import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

Rectangle {
    id: viewGroove;
    height:  50;
    width: 100;
    color: "#1f1f1f";
    radius: 3;

    border {
        width: 1;
        color: "#0e0e0e";
    }

    CustomBorder {
        gradient: Gradient {
            GradientStop {position: 0.0; color: "transparent";}
            GradientStop {position: 0.9; color: "transparent";}
            GradientStop {position: 1.0; color: "#4e4e51";}
        }
    }

    ExclusiveGroup {
        id: exclusiveGroup;
    }

    DropShadow {
        anchors.fill: source;
        horizontalOffset: 1
        visible: !tableButton.checked;
        verticalOffset: 4
        radius: 16.0
        samples: radius * 2;
        color: "#e0000000"
        source: tableButton;
        transparentBorder: true;
    }

    DropShadow {
        anchors.fill: source;
        visible: !gridButton.checked;
        horizontalOffset: 2;
        verticalOffset: 4;
        radius: 8.0
        samples: radius * 2;
        color: "#b0000000"
        source: gridButton;
        transparentBorder: true;
    }

    Rectangle {
        id: tableButton;
        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
        }
        width: parent.width / 2;
        radius: 3;
        property bool checked: false;
        property ExclusiveGroup exclusiveGroup: exclusiveGroup;
        onExclusiveGroupChanged: {
                if (exclusiveGroup) {
                    exclusiveGroup.bindCheckable(tableButton);
                }
        }
        border.color: "black";
        opacity: checked ? 0 : 1.0;

        gradient: Gradient {
            GradientStop {position: 0.0; color: "#4f4f4f";}
            GradientStop {position: 1.0; color: "#3b3b3b";}
        }

        Rectangle {
            id: innerColor;
            anchors {
                fill: parent;
                margins: 2;
            }
            radius: parent.radius;
            opacity: parent.opacity;
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    windowStack.currentItem.stackId.push({item: gameTable, replace: true, immediate: true});
                    tableButton.checked = true;
                }
            }

            property Gradient _gradient: Gradient {
                GradientStop {position: 0.0; color: "#2e2e2e";}
                GradientStop {position: 0.5; color: "#252626";}
                GradientStop {position: 0.8; color: "#212121";}
                GradientStop {position: 1.0; color: "#1d1d1d";}
            }

            gradient: {
                if (parent.checked) {
                    return undefined;
                }
                return _gradient;
            }
        }
    }

    Image {
        z: tableButton.z + 1;
        height: 12;
        width: 12;
        anchors.centerIn: tableButton;
        source: tableButton.checked  ? "../assets/table-orange.png" : "../assets/table.png";
        sourceSize {
            height: 12;
            width: 12;
        }
    }

    Rectangle {
        id: gridButton;
        anchors {
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
        }
        width: parent.width / 2;
        radius: 3;
        property bool checked: true;
        property ExclusiveGroup exclusiveGroup: exclusiveGroup;
        onExclusiveGroupChanged: {
                if (exclusiveGroup) {
                    exclusiveGroup.bindCheckable(gridButton);
                }
        }
        opacity: checked ? 0 : 1.0;
        border.color: "black"

        gradient: Gradient {
            GradientStop {position: 0.0; color: "#4f4f4f";}
            GradientStop {position: 1.0; color: "#3b3b3b";}
        }

        Rectangle {
            anchors {
                fill: parent;
                margins: 2;
            }
            radius: parent.radius;
            opacity: parent.opacity;
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    windowStack.currentItem.stackId.push({item: gameGrid, replace: true, immediate: true});
                    gridButton.checked = true;
                }
            }

            property Gradient _gradient: Gradient {
                GradientStop {position: 0.0; color: "#2e2e2e";}
                GradientStop {position: 0.5; color: "#252626";}
                GradientStop {position: 0.8; color: "#212121";}
                GradientStop {position: 1.0; color: "#1d1d1d";}
            }

            gradient: {
                if (parent.checked) {
                    return undefined;
                }
                return _gradient;
            }
        }
    }

    Image {
        z: gridButton.z + 1;
        height: 12;
        width: 12;
        anchors.centerIn: gridButton;
        source: gridButton.checked ?  "../assets/grid-orange.png" : "../assets/grid.png";
        sourceSize {
            height: 12;
            width: 12;
        }
    }
}
