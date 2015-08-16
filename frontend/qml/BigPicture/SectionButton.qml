import QtQuick 2.4
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0


Item  {
    id: sectionButton;

    property string text: "";
    property bool checked: false;
    property ExclusiveGroup exclusiveGroup: null

    onExclusiveGroupChanged: {
        if (exclusiveGroup) {
            exclusiveGroup.bindCheckable( sectionButton );
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {
            sectionButton.checked = true;
        }
    }

    DropShadow {
        anchors.fill: source;
        source: section;
        verticalOffset: sectionButton.checked ? 3 : 1;
        horizontalOffset: 0;
        color: "black";
        transparentBorder: true;
        radius: sectionButton.checked ? 16 : 4;
        samples: radius * 2;
        cached: true;

        Behavior on verticalOffset {
            PropertyAnimation {
                duration: 300;
            }
        }

        Behavior on radius {
            PropertyAnimation {
                duration: 300;
            }
        }
    }

    Rectangle {
        id: section;
        anchors.fill: parent;
        color: "#262626";

        Text {
            anchors.centerIn: parent;
            text: sectionButton.text;
            font {
                pixelSize: 16;
                bold: true;
            }
            color: sectionButton.checked ? "white" : "darkgray";

            Behavior on color {
                PropertyAnimation {
                    duration: 300;
                }
            }
        }

        Rectangle {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: 2;
            color: "white";
            opacity: 0.03;
        }

        Rectangle {
            anchors {
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
            }
            height: 2;
            color: "black";
            opacity: 0.2;
        }
    }
}

