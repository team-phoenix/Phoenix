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

    property string screenIcon: {
        if ( root.visibility === Window.FullScreen ) screenIcon: "window.svg";
        else if ( root.visibility === Window.Windowed | Window.Maximized ) screenIcon: "fullscreen.svg";
    }

    Rectangle {
        id: headerArea;
        anchors { top: parent.top; left: parent.left; right: parent.right; }
        color: "transparent"
        height: 70;

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

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; }
                border { width: 1; color: "#FFFFFF"; }
                color: "transparent";
                height: 8;
                width: height;
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

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; }
                border { width: 1; color: "#FFFFFF"; }
                color: "transparent";
                height: 12;
                width: height;
            }

            // Fullscreen
            Rectangle {
                anchors { top: parent.top; bottom: parent.bottom; }
                color: "transparent";
                width: 24;

                Image {
                    anchors.centerIn: parent;
                    height: 24;
                    width: 24;
                    sourceSize { height: height; width: width; }
                    source: screenIcon;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                                if ( root.visibility === Window.FullScreen )
                                    root.visibility = Window.Windowed;
                                else if ( root.visibility === Window.Windowed | Window.Maximized )
                                    root.visibility = Window.FullScreen;
                        }
                    }
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

    StackView {
        id: contentAreaStackView;
        initialItem: boxArtGridComponent;
        anchors.fill: parent;
        anchors.bottomMargin: root.gameViewObject.coreControl.state === Control.PAUSED ? gameSuspendedArea.height : 0;

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
        visible: root.gameViewObject.coreControl.state === Control.PAUSED;
    }
}
