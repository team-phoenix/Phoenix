import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: contentArea;
    color: PhxTheme.common.secondaryBackgroundColor;

    property alias contentLibraryModel: libraryModel;
    property alias contentStackView: contentAreaStackView;
    property alias contentBoxartGrid: boxArtGridComponent
    property alias contentInputView: inputView;
    property alias contentSlider: zoomSlider;
    property alias boxartGrid: boxArtGridComponent;

    property string screenIcon: {
        if ( root.visibility === Window.FullScreen ) screenIcon: "window.svg";
        else if ( root.visibility === Window.Windowed | Window.Maximized ) screenIcon: "fullscreen.svg";
    }

    Rectangle {
        id: headerArea;
        anchors { top: parent.top; left: parent.left; right: parent.right;}
        color: "transparent";
        z: 100;
        height: 95;
        opacity: .75;

        Rectangle {
            anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 35; }
            color: "transparent";
            border.width: 1;
            border.color: "#FFFFFF";
            width: 250;
            radius: height/2;
            height: 30;

            PhxSearchBar {
                anchors { left: parent.left; leftMargin: 10; }
                id: searchBar;
                font.pixelSize: 14;
                placeholderText: "";
                width: parent.width - 50;
                height: parent.height;
                textColor: "#FFFFFF";

                Timer {
                    id: searchTimer;
                    interval: 300;
                    running: false;
                    repeat: false;
                    onTriggered: { libraryModel.setFilter( "games", "title", '%' + searchBar.text + '%' ); }
                }
                onTextChanged: searchTimer.restart();
            }

            Button {
                anchors { right: parent.right; rightMargin: searchBar.anchors.leftMargin; verticalCenter: parent.verticalCenter; }
                width: 20;
                height: width;
                smooth: true;
                iconSource: searchBar.text === "" ? "search.svg" : "del.svg";
                style: ButtonStyle { background: Rectangle { color: "transparent"; } }
                onClicked: searchBar.text = "";
            }
        }

        Row {
            anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 30; }
            spacing: 12;

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; }
                border { width: 1; color: "white"; }
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
                            color: "white";
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
                border { width: 1; color: "white"; }
                color: "transparent";
                height: 12;
                width: height;
            }

            // Fullscreen
            Rectangle {
                anchors { top: parent.top; bottom: parent.bottom; }
                color: "transparent";
                width: 40;

                Button {
                    anchors.centerIn: parent;
                    width: parent.width;
                    iconName: screenIcon;
                    iconSource: screenIcon;
                    style: ButtonStyle { background: Rectangle { color: "transparent"; } }
                    onClicked: {
                            if ( root.visibility === Window.FullScreen )
                                root.visibility = Window.Windowed;
                            else if ( root.visibility === Window.Windowed | Window.Maximized )
                                root.visibility = Window.FullScreen;
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

        function dragEvent( drag ) {
            if ( drag.hasUrls ) {
                handleDraggedUrls( drag.urls );
            }
        }

        function dropEvent( drop ) {
            handleDroppedUrls();
        }

        function containsEvent() {
            handleContainsDrag( rootDropArea.containsDrag );
        }

        Component.onCompleted: {
            rootDropArea.onEntered.connect( dragEvent );
            rootDropArea.onDropped.connect( dropEvent );
            rootDropArea.onContainsDragChanged.connect( containsEvent );

            libraryModel.startWorkerThread();
        }
    }

    StackView {
        id: contentAreaStackView;
        initialItem: boxArtGridComponent;

        anchors.fill: parent;

        delegate: StackViewDelegate {
            function transitionFinished( properties )
            {
                properties.exitItem.opacity = 1;
            }

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
        id: inputView;
        InputView { objectName: "InputView" }
    }
}
