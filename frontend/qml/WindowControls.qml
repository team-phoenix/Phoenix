import QtQuick 2.6
import QtQuick.Window 2.0

import vg.phoenix.themes 1.0

Item {
    id: windowControls;
    property alias spacing: controlRow.spacing;

    Row {
        id: controlRow;
        spacing: 12;
        //anchors.fill: parent;
        anchors {
            right: parent.right;
        }

        // Fullscreen
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter;
            height: windowControls.height + 2;
            width: windowControls.height;
            color: fullscreenButton.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
            radius: width * 0.5;

            Image {
                id: fullscreenButton;
                anchors.centerIn: parent;
                height: window.visibility === Window.FullScreen ? 18 : 24;
                width: height;

                sourceSize { height: height; width: width; }

                property string screenIcon: {
                    if ( window.visibility === Window.FullScreen ) screenIcon: "qrc:/Assets/window.svg";
                    else if ( window.visibility === Window.Windowed | Window.Maximized ) screenIcon: "qrc:/Assets/fullscreen.svg";
                }
                source: screenIcon;

                activeFocusOnTab: true;

                MouseArea {
                    anchors.fill: parent;
                    onClicked: window.toggleFullscreen();
                }

                Keys.onPressed: {
                    if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                        window.toggleFullscreen();
                        event.accepted = true;
                    }
                }
            }
        }


        // Close
        Image {
            id: closeButton;
            anchors.verticalCenter: parent.verticalCenter;
            height: 14;
            width: height;

            visible: window.visibility === Window.FullScreen;
            enabled: visible;

            activeFocusOnTab: enabled;

            sourceSize { height: height; width: width; }
            source: "qrc:/Assets/close.svg";

            MouseArea {
                anchors.fill: parent;
                onClicked: Qt.quit();
            }

            Keys.onPressed: {
                if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                    Qt.quit();
                    event.accepted = true;
                }
            }

            Rectangle {
                anchors.fill: parent;
                color: "transparent";
                border.color: closeButton.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
                border.width: 2;
            }
        }

    }
}
