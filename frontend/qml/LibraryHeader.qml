import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

import "../"
import "qrc:/Emulator"
import "qrc:/Frontend"
import "qrc:/Util"

Item {
    property alias searchBar: searchBar;
    property alias zoomSlider: zoomSlider;

    Rectangle {
        anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 30; }
        color: "#FFF";
        width: 250;
        radius: height / 2;
        height: 30;
        border.color: searchBar.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
        border.width: 2;

        PhxSearchBar {
            anchors { left: parent.left; leftMargin: 10; }
            id: searchBar;
            font.pixelSize: PhxTheme.common.baseFontSize;
            placeholderText: "";
            width: parent.width - 50;
            height: parent.height;
            textColor: "#333";

            Timer {
                id: searchTimer;
                interval: 300;
                running: false;
                repeat: false;
                onTriggered: { library.libraryModel.setFilter( "title", "%" + searchBar.text + "%" ); }
            }
            onTextChanged: searchTimer.restart();
        }

        Image {
            anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter; }
            width: 20;
            height: width;
            sourceSize { height: height; width: width; }
            source: searchBar.text === "" ? "qrc:/Assets/search.svg" : "qrc:/Assets/del.svg";
            MouseArea {
                anchors.fill: parent;
                onClicked: searchBar.text = "";
            }
        }
    }

    Row {
        anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 30; }
        spacing: 12;

        // Small rectangle
        Item {
            anchors { verticalCenter: parent.verticalCenter; }
            width: 12;
            height: parent.height;

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; right: parent.right; }
                border { width: 1; color: "#FFFFFF"; }
                color: "transparent";
                width: height;
                height: 8;

            }
            MouseArea {
                anchors.fill: parent;
                onClicked: zoomSlider.value = Math.max( zoomSlider.minimumValue, zoomSlider.value - zoomSlider.stepSize );
            }
        }

        Slider {
            id: zoomSlider;
            anchors { verticalCenter: parent.verticalCenter; }
            width: 100;
            height: 30;
            minimumValue: 100;
            maximumValue: 450;
            value: 150;
            stepSize: 50;
            activeFocusOnPress: true;
            tickmarksEnabled: false;

            style: SliderStyle {
                handle: Item {
                    width: 6;
                    height: 10;

                    transform: Scale {
                        origin.y: 10 / 2;
                        yScale: zoomSlider.activeFocus ? 1.25 : 1.0;
                        Behavior on yScale { PropertyAnimation { duration: 250; } }
                    }

                    Rectangle {
                        id: handleRectangle;
                        anchors.fill: parent;
                        color: zoomSlider.activeFocus ? PhxTheme.common.menuItemHighlight : "#FFFFFF";

                        Behavior on color { ColorAnimation { duration: 250; } }
                    }
                }

                groove: Item {
                    width: control.width;
                    height: 1.5;

                    Rectangle {
                        anchors.fill: parent;
                        color: "#FFFFFF";
                        opacity: .35;
                    }
                }
            }
        }

        // Large rectangle
        Item {
            anchors { verticalCenter: parent.verticalCenter; }
            width: 12;
            height: parent.height;

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; }
                border { width: 1; color: "#FFFFFF"; }
                color: "transparent";
                height: 12;
                width: height;
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: zoomSlider.value = Math.min( zoomSlider.maximumValue, zoomSlider.value + zoomSlider.stepSize );
            }
        }

        // Spacer
        Item { width: 12; height: 12; }

        // Fullscreen
        Rectangle {
            anchors.verticalCenter: parent.verticalCenter;
            height: 32;
            width: height;
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
            width: 14;

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
