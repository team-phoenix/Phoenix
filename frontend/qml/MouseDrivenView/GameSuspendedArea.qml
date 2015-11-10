import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.cache 1.0
import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

// Suspended game section
Rectangle {
    id: gameSuspendedArea;
    color: PhxTheme.common.gameSuspendedBackgroundColor;
    // opacity: .95;

    Row {
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left; }
        width: PhxTheme.common.menuWidth;

        spacing: 8;

        Rectangle {
            id: gameSuspendedSec;
            anchors { top: parent.top; bottom: parent.bottom; }
            width: parent.width;

            color: "transparent";

            Row {
                anchors { top: parent.top; bottom: parent.bottom; }
                width: parent.width;

                spacing: 8;

                Image {
                    id: gridItemImage;
                    anchors { verticalCenter: parent.verticalCenter; }
                    width: 48;
                    height: 48;
                    visible: true;
                    asynchronous: true;
                    source: root.gameViewObject.artworkURL === "" ? "noartwork.png" : root.gameViewObject.artworkURL;
                    sourceSize { height: height; width: width; }
                    fillMode: Image.PreserveAspectFit;
                }

                MarqueeText {
                    anchors { verticalCenter: parent.verticalCenter; }
                    width: parent.width - 72;

                    horizontalAlignment: Text.AlignLeft;
                    fontSize: PhxTheme.common.baseFontSize + 1;
                    color: PhxTheme.common.baseBackgroundColor;
                    spacing: 40;

                    text: root.gameViewObject.title;

                    running: gameSuspendedMouseArea.containsMouse;
                    pixelsPerFrame: 2.0;
                }
            }

            MouseArea {
                id: gameSuspendedMouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    // Prevent user from clicking on anything while the transition occurs
                    root.disableMouseClicks();

                    // Destroy the compenent this MouseArea lives in
                    layoutStackView.pop();
                }
                onContainsMouseChanged: checkMouse();

                Connections {
                    target: root.layoutStackView;
                    onTransitioningChanged: gameSuspendedMouseArea.checkMouse();
                }

                function checkMouse() {
                    if( containsMouse ) {
                        gameSuspendedSec.color = PhxTheme.common.gameSuspendedHoverBackgroundColor;
                        rootMouseArea.cursorShape = Qt.PointingHandCursor;
                    } else {
                        gameSuspendedSec.color = "transparent";
                        rootMouseArea.cursorShape = Qt.ArrowCursor;
                    }
                }
            }
        }

        Item {
            anchors { top: parent.top; bottom: parent.bottom; }
            width: 32;

            // Play
            Image {
                id: playImage;
                anchors { verticalCenter: parent.verticalCenter; }
                // anchors.margins: 10;
                width: parent.width;
                fillMode: Image.PreserveAspectCrop;

                source: "play.svg";
                sourceSize { height: height; width: width; }
            }

            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: { rootMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { rootMouseArea.cursorShape = Qt.ArrowCursor; }
                onClicked: {
                    // Prevent user from clicking on anything while the transition occurs
                    root.disableMouseClicks();

                    // Resume game
                    root.gameViewObject.coreControl.play();

                    // Destroy the compenent this MouseArea lives in
                    layoutStackView.pop();
                }
            }
        }
    }

    Row {
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }

        Item {
            anchors { top: parent.top; bottom: parent.bottom; }
            width: 40;

            // Close
            Image {
                anchors.centerIn: parent;
                width: 14;
                height: 14;
                source: "close.svg";
                sourceSize { height: height; width: width; }
            }
            MouseArea {
                anchors.fill: parent;
                hoverEnabled: true;
                onEntered: { rootMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { rootMouseArea.cursorShape = Qt.ArrowCursor; }
                onClicked: { root.resetTitle(); root.gameViewObject.coreControl.stop(); }
            }
        }
    }
}
