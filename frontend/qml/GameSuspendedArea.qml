import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

// Suspended game section
Rectangle {
    id: gameSuspendedArea;
    height: 65;
    color: PhxTheme.common.gameSuspendedBackgroundColor;
    Layout.fillWidth: true;
    z: parent.z + 1;
    anchors { bottom: parent.bottom }

    /* Rectangle {
        anchors { top: parent.top; topMargin: 0; }
        width: parent.width;
        height: 1;
        color: Qt.rgba(255,255,255,.1);
    } */

    Row {
        anchors.fill: parent;
        spacing: 0;

        Rectangle {
            id: gameSuspendedSec;
            anchors { top: parent.top; bottom: parent.bottom; }
            width: PhxTheme.common.menuWidth;
            color: "transparent";

            Image {
                id: gridItemImage;
                anchors { bottomMargin: 8; leftMargin: 8; left: parent.left; right: parent.right; bottom: parent.bottom; }
                visible: true;
                asynchronous: true;
                width: 48;
                height: 48;
                horizontalAlignment: Image.AlignLeft;
                source: "noartwork.png";
                sourceSize { height: height; width: width; }
                fillMode: Image.PreserveAspectFit;
            }

            Label {
                anchors { left: parent.left; leftMargin: 64; verticalCenter: parent.verticalCenter; }
                elide: Text.ElideRight;
                width: PhxTheme.common.menuWidth - 72;
                text: root.gameViewObject.coreGamePair[ "title" ];
                color: PhxTheme.normalFontColor;
                font.pixelSize: PhxTheme.common.suspendedGameFontSize;
            }

            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    // Prevent user from clicking on anything while the transition occurs
                    root.disableMouseClicks();

                    // Destroy the compenent this MouseArea lives in
                    layoutStackView.pop();
                }
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
                onEntered: {
                    parent.color = PhxTheme.common.gameSuspendedHoverBackgroundColor;
                    rootMouseArea.cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.color = "transparent" // "PhxTheme.common.suspendedGameBackgroundColor";
                    rootMouseArea.cursorShape = Qt.ArrowCursor;
                }
            }
        }

        // Play button
        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            color: "transparent";
            width: 32;
            Image {
                anchors.centerIn: parent;
                anchors.margins: 10;
                width: parent.width;
                source:  "play.svg";
                sourceSize { height: height; width: width; }
            }
            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
                onClicked: {
                    // Prevent user from clicking on anything while the transition occurs
                    root.disableMouseClicks();

                    // Destroy the compenent this MouseArea lives in
                    layoutStackView.pop();
                }
                onEntered: { rootMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { rootMouseArea.cursorShape = Qt.ArrowCursor; }
            }
        }
    }

    Row {
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }

        Rectangle {
            anchors { top: parent.top; bottom: parent.bottom; }
            width: 40;
            color: "transparent";

            // Close
            Image {
                anchors.centerIn: parent;
                width: 16;
                height: 16;
                source: "close.svg";
                sourceSize { height: height; width: width; }
            }
            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: { rootMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { rootMouseArea.cursorShape = Qt.ArrowCursor; }
                onClicked: { root.gameViewObject.videoItem.slotStop(); }
            }
        }
    }
}
