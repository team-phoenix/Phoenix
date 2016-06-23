import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0

import vg.phoenix.themes 1.0

Item {
    property int combinedWidth: fullscreenButtonContainer.fullscreenButtonWidth +
                                ( window.visibility === Window.FullScreen ?
                                     controlsRow.spacing + closeButton.closeButtonWidth : 0 );

    RowLayout {
        id: controlsRow;
        spacing: 12;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        // Fullscreen
        Rectangle {
            id: fullscreenButtonContainer;
            property int fullscreenButtonWidth: window.visibility === Window.FullScreen ? 18 : 24;

            Layout.preferredHeight: fullscreenButtonWidth;
            Layout.preferredWidth: fullscreenButtonWidth;

            color: fullscreenButton.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
            radius: width * 0.5;

            Image {
                id: fullscreenButton;
                anchors.top: parent.top;
                anchors.bottom: parent.bottom;
                width: parent.width;

                sourceSize { width: width; height: height; }

                source: screenIcon;
                property string screenIcon: {
                    if ( window.visibility === Window.FullScreen ) screenIcon: "../Assets/window.svg";
                    else if ( window.visibility === Window.Windowed | Window.Maximized ) screenIcon: "../Assets/fullscreen.svg";
                }

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

            property int closeButtonWidth: 14;

            Layout.preferredHeight: closeButtonWidth;
            Layout.preferredWidth: closeButtonWidth;

            visible: window.visibility === Window.FullScreen;
            enabled: visible;

            activeFocusOnTab: enabled;

            sourceSize { height: height; width: width; }
            source: "../Assets/close.svg";

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
