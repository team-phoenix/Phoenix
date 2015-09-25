import QtQuick 2.5
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.cache 1.0
import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Item {

    DropArea {
        id: rootDropArea;
        anchors.fill: parent;
    }

    ColumnLayout {
        anchors.fill: parent;

        RowLayout {
            id: gameSelectionPane;
            anchors { fill: parent }
            spacing: 0;
            anchors.bottomMargin: root.gameViewObject.videoRender.coreState === Core.STATEPAUSED ? gameSuspendedSection.height : 0;

            SelectionArea {
                id: selectionArea
                anchors { top: parent.top; bottom: parent.bottom }
                width: 250;
                z: contentArea.z + 1
            }

            ContentArea {
                id: contentArea
                anchors { top: parent.top; bottom: parent.bottom }
                Layout.fillWidth: true
            }
        }

        // Suspended game section
        Rectangle {
            id: gameSuspendedSection;
            color: PhxTheme.common.highlightBackgroundColor;
            Layout.fillWidth: true;
            height: 65;
            z: 100;
            anchors { bottom: parent.bottom }
            visible: root.gameViewObject.videoRender.coreState === Core.STATEPAUSED;


            Rectangle {
                // visible: root.gameViewObject.videoRender.coreState === Core.STATEPAUSED;
                anchors { top: parent.top; bottom: parent.bottom; }
                width: 250;
                color: "transparent";

                Image {
                    id: gridItemImage;
                    anchors { bottomMargin: 8; leftMargin: 8; left: parent.left; right: parent.right; bottom: parent.bottom; }
                    visible: true;
                    asynchronous: true;
                    width: 48;
                    height: 48;
                    horizontalAlignment: Image.AlignLeft;
                    source: "missingArtwork.png";
                    sourceSize { width: 400; height: 400; }
                    fillMode: Image.PreserveAspectFit;
                }

                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: 64;
                        verticalCenter: parent.verticalCenter;
                    }
                    elide: Text.ElideRight;
                    width: 178;
                    text: root.gameViewObject.coreGamePair[ "title" ];
                    color: PhxTheme.normalFontColor;
                    font.pixelSize: PhxTheme.selectionArea.headerFontSize;
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
                        parent.color = PhxTheme.common.highlightHoverBackgroundColor;
                        rootMouseArea.cursorShape = Qt.PointingHandCursor;
                    }
                    onExited: {
                        parent.color = PhxTheme.common.highlightBackgroundColor;
                        rootMouseArea.cursorShape = Qt.ArrowCursor;
                    }
                }
            }

           /* Button {
                 anchors { left: parent.left; leftMargin: 100; }
                 height: width;
                 width: parent.width;
                 iconName: "Play";
                 iconSource: "play.svg";
                 style: ButtonStyle { background: Rectangle { color: "transparent"; } }
             } // what happened to the rectangle?
             */


            Row {

                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    right: parent.right;
                }

                Rectangle {
                    anchors {
                        top: parent.top;
                        bottom: parent.bottom;
                    }

                    width: 50;
                    color: "transparent";

                   Button {
                        anchors.centerIn: parent;
                        width: parent.width;
                        iconName: "Close";
                        iconSource: "close.svg";
                        style: ButtonStyle { background: Rectangle { color: "transparent"; } }
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            root.gameViewObject.videoRender.stop();
                        }
                    }

                }
            }
        }
    }
}
