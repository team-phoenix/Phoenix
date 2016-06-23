import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import Phoenix.Backend 1.0
import Phoenix.Launcher 1.0
import Phoenix.Paths 1.0

import Phoenix.Emulator 1.0
import Phoenix.Settings 1.0
import Phoenix.Theme 1.0
import Phoenix.Util 1.0

Item {
    property alias searchBar: searchBar;
    property alias zoomSlider: zoomSlider;

    Rectangle {
        anchors { verticalCenter: parent.verticalCenter; left: parent.left; }
        color: "#FFF";
        width: 250;
        radius: height / 2;
        height: parent.height * 0.75;
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
            width: parent.height * 0.6;
            height: width;
            sourceSize { height: height; width: width; }
            source: searchBar.text === "" ? "../Assets/search.svg" : "../Assets/del.svg";
            MouseArea {
                anchors.fill: parent;
                onClicked: searchBar.text = "";
            }
        }
    }

    RowLayout {
        anchors { top: parent.top; bottom: parent.bottom; right: parent.right; }
        spacing: 12;

        // Small rectangle
        Item {
            Layout.fillHeight: true;
            Layout.preferredWidth: 12;

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
            Layout.preferredWidth: 100;
            Layout.fillHeight: true;

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
            Layout.fillHeight: true;
            Layout.preferredWidth: 12;

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

        // A spacer
        Item {
            Layout.preferredHeight: 1;
            Layout.preferredWidth: 1;
        }

        WindowControls {
            Layout.fillHeight: true;
            Layout.preferredWidth: combinedWidth;
        }

    }
}
