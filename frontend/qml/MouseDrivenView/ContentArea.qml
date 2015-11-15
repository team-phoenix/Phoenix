import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Rectangle {
    id: contentArea;
    color: PhxTheme.common.secondaryBackgroundColor;

    property alias contentLibraryModel: libraryModel;
    property alias contentStackView: contentAreaStackView;
    property alias contentBoxartGrid: boxArtGridComponent
    property alias contentLibrarySettingsView: librarySettingsView;
    property alias contentInputSettingsView: inputSettingsView;
    property alias contentSlider: zoomSlider;
    property alias boxartGrid: boxArtGridComponent;

    property bool fullscreen: root.visibility === Window.FullScreen;
    property string screenIcon: {
        if( fullscreen ) screenIcon: "window.svg";
        else screenIcon: "fullscreen.svg";
    }

    Rectangle {
        id: headerArea;
        anchors { top: parent.top; left: parent.left; right: parent.right; }
        color: "transparent"
        height: 70;
        z: 1;

        Rectangle {
            anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 30; }
            color: "#FFF";
            width: 250;
            radius: height/2;
            height: 30;

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
                    onTriggered: { libraryModel.setFilter( "games", "title", "%" + searchBar.text + "%" ); }
                }
                onTextChanged: searchTimer.restart();
            }

            Image {
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter; }
                width: 20;
                height: width;
                sourceSize { height: height; width: width; }
                source: searchBar.text === "" ? "search.svg" : "del.svg";
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
                minimumValue: 150;
                maximumValue: 450;
                value: 150;
                stepSize: 50;
                activeFocusOnPress: true;
                tickmarksEnabled: false;

                style: SliderStyle {
                    handle: Item {
                        height: 10;
                        width: 6;

                        Rectangle {
                            id: handleRectangle;
                            anchors.fill: parent;
                            color: "#FFFFFF";
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
            Image {
                anchors.verticalCenter: parent.verticalCenter;
                height: fullscreen ? 18 : 24;
                width: height;

                sourceSize { height: height; width: width; }
                source: screenIcon;

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if( fullscreen ) root.visibility = Window.Windowed;
                        else root.visibility = Window.FullScreen;
                    }
                }
            }

            // Close
            Image {
                anchors.verticalCenter: parent.verticalCenter;
                height: 14;
                width: 14;

                visible: root.visibility === Window.FullScreen;
                enabled: visible;

                sourceSize { height: height; width: width; }
                source: "close.svg";

                MouseArea {
                    anchors.fill: parent;
                    onClicked: Qt.quit();
                }
            }
        }

//        Column {
//            anchors {
//                left: parent.left;
//                right: parent.right;
//                bottom: parent.bottom;
//            }

//            Rectangle {
//                anchors {
//                    left: parent.left;
//                    right: parent.right;
//                }
//                height: 1;
//                color: "white";
//                opacity: 0.05;
//            }

//            Rectangle {
//                anchors {
//                    left: parent.left;
//                    right: parent.right;
//                }
//                height: 2;
//                color: "black";
//                opacity: 0.2;
//            }
//        }
    }

    /*
    HeaderBar {
        id: headerBar;
        z: 100;
        height: 36;
        state: "SELECTION";

        anchors {
            left: parent.left
            right: parent.right;
            top: parent.top;

            topMargin: 24;
            leftMargin: 12;
            rightMargin: 12;
        }

        visible: {
            if ( contentAreaStackView.currentItem !== null ) {
                return contentAreaStackView.currentItem.objectName === "BoxartGridView";
            }
            return false;
        }


        MouseArea {
            anchors.fill: parent;
            onClicked: headerBar.state = "PLAYING_GAME";
        }
    }
    */

    LibraryModel {
        id: libraryModel;
        function dragEvent( drag ) { if ( drag.hasUrls ) { handleDraggedUrls( drag.urls ); } }
        function dropEvent( drop ) { handleDroppedUrls(); }
        function containsEvent() { handleContainsDrag( rootDropArea.containsDrag ); }

        Component.onCompleted: {
            rootDropArea.onEntered.connect( dragEvent );
            rootDropArea.onDropped.connect( dropEvent );
            rootDropArea.onContainsDragChanged.connect( containsEvent );
            libraryModel.startGameScannerThread();
        }
    }

    property bool currentlySuspended: typeof root.gameViewObject === 'undefined' ?
                                      false : root.gameViewObject.coreControl.state === Control.PAUSED;

    StackView {
        id: contentAreaStackView;
        initialItem: boxArtGridComponent;
        anchors.fill: parent;
        anchors.bottomMargin: currentlySuspended ? gameSuspendedArea.height : 0;

        delegate: StackViewDelegate {
            function transitionFinished( properties ) { properties.exitItem.opacity = 1; }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem;
                    property: "opacity";
                    from: 0;
                    to: 1;
                }

                PropertyAnimation {
                    target: exitItem;
                    property: "opacity";
                    from: 1;
                    to: 0;
                }
            }
        }
    }

    Component {
        id: boxArtGridComponent;
        BoxartGridView {
            id: boxartGrid;
            objectName: "BoxartGridView";
            color: "transparent";
        }
    }

    Component {
        id: detailGameViewComponent;
        DetailedGameView { }
    }

    Component {
        id: inputSettingsView;
        InputSettingsView { objectName: "InputSettingsView"; }
    }

    Component {
        id: librarySettingsView;
        LibrarySettingsView { objectName: "LibrarySettingsView"; }

    }

    GameSuspendedArea {
        id: gameSuspendedArea;
        objectName: "GameSuspendedArea";
        visible: currentlySuspended;
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
        height: 65;
    }
}
