import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

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

    Rectangle {
        id: headerArea;
        anchors { top: parent.top; left: parent.left; right: parent.right;}
        color: "transparent";
        z: 100;
        height: 95;
        Rectangle {
            anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 40; }
            color: "transparent";
            border.width: 2;
            border.color: "#75FFFFFF";
            width: 250;
            height: 35;

            PhxSearchBar {
                anchors { left: parent.left; leftMargin: 10; }
                id: searchBar;
                font.pixelSize: 14;
                placeholderText: "";
                width: parent.width - 50;
                height: parent.height;
                textColor: "#95FFFFFF";

                Timer {
                    id: searchTimer;
                    interval: 300;
                    running: false;
                    repeat: false;
                    onTriggered: { libraryModel.setFilter( "games", "title", '%' + searchBar.text + '%' ); }
                }
                onTextChanged: searchTimer.restart();
            }
            Image {
                anchors { right: parent.right; rightMargin: 5; verticalCenter: parent.verticalCenter; }
                width: 20;
                height: width;
                fillMode: Image.PreserveAspectFit;
                source: "search.svg"
            }
        }

        Column {
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
        }

        Row {
            anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 30;
            }
            spacing: 12;

            Rectangle {
                anchors { verticalCenter: parent.verticalCenter; }
                border { width: 2; color: "white"; }
                color: "transparent";
                height: 10;
                width: height;
            }

            Slider {
                id: zoomSlider;
                anchors { verticalCenter: parent.verticalCenter; }
                width: 100;
                height: 30;
                minimumValue: 150;
                maximumValue: 400;
                value: minimumValue;
                stepSize: 10;
                activeFocusOnPress: true;
                tickmarksEnabled: false;

                style: SliderStyle {
                    handle: Item {
                        height: 15;
                        width: 8;

                        Rectangle {
                            id: handleRectangle;
                            anchors.fill: parent;
                            color: "white";
                        }
                    }

                    groove: Item {
                        width: control.width;
                        height: 2;

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
                border { width: 2; color: "white"; }
                color: "transparent";
                height: 15;
                width: height;
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
