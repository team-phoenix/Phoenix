import QtQuick 2.3
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Rectangle {

    // This is the drop area that Phoenix shows, in place of the grid,
    // when there are 0 games selected for any console.

    // This area informs the user that they can either drop games onto this screen,
    // or they can click the 'browse' text in order to open up a dialog screen, in
    // order to import games.

    color: "transparent";
    border {
        width: 1;
        color: "black";
    }

    radius: 17;

    FileDialog {
        id: pathDialog;
        selectFolder: true;
        title: "Add Game Folder";
        onAccepted: {
            phoenixLibrary.startAsyncScan(fileUrl);
        }
    }

    DropShadow {
        anchors.fill: source;
        source: header;
        verticalOffset: 1;
        horizontalOffset: 0;
        color: "white";
        opacity: 0.2;
        radius: 1;
        samples: radius * 2;
    }

    Text {
        id: header;
        color: "black";
        opacity: 0.6
        text: "Drag & Drop";
        anchors {
            centerIn: parent;
            verticalCenterOffset: -36;
        }

        font {
            family: "Sans";
            pixelSize: 56;
            bold: true;
        }
    }

    Text {
        font {
            family: "Sans";
            pixelSize: 24;
        }
        anchors {
            top: header.bottom;
            topMargin: 6;
            horizontalCenter: parent.horizontalCenter;
        }

        color: "white";
        opacity: 0.20
        text: "games, or <b>browse<b>";
        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onEntered: parent.opacity = 0.3;
            onExited: parent.opacity = 0.2;
            onClicked: pathDialog.open();
        }
    }

    Rectangle {
        id: sourceMask;
        color: "transparent";
        anchors {
            fill: parent;
            margins: 1;
        }
        border {
            width: 16;
            color: "black";
        }
        opacity: 0.25;
        radius: parent.radius;

        Rectangle {
            color: "transparent";
            anchors {
                fill: parent;
                margins: parent.border.width;
            }
            opacity: 0.25;
            border {
                width: 1;
                color: "white";
            }
            radius: parent.radius - parent.border.width;
        }
    }
}
