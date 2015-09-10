import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: contentArea;
    height: 200;
    width: 200;
    color: PhxTheme.common.primaryBackgroundColor;

    property alias contentLibraryModel: libraryModel;
    property alias contentStackView: contentAreaStackView;
    property alias contentBoxartGrid: boxArtGridComponent
    property alias contentInputView: inputView;

    property alias contentSlider: zoomSlider;

    property alias boxartGrid: boxArtGridComponent;

    Rectangle {
        id: headerArea;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        color: PhxTheme.common.primaryBackgroundColor;
        z: 100;

        height: 40;



        PhxSearchBar {
            id: searchBar;
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: 6;
            }

            placeholderText: "Search..."
            width: 200;
            textColor: "white";

            Timer {
                id: searchTimer;
                interval: 300;
                running: false;
                repeat: false;
                onTriggered: {
                    libraryModel.setFilter( "games", "title", '%' + searchBar.text + '%' );
                }

            }
            onTextChanged: searchTimer.restart();

        }

        Column {

            anchors {
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                height: 1;
                color: "white";
                opacity: 0.05;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                height: 2;
                color: "black";
                opacity: 0.2;
            }

        }

        Row {
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
                rightMargin: 12;
            }

            spacing: 12;

            Rectangle {
                color: "transparent";
                border {
                    width: 2;
                    color: "white";
                }

                height: 7;
                width: height;

                anchors {
                    verticalCenter: parent.verticalCenter;
                }
            }

            Slider {
                id: zoomSlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }

                width: 150;

                minimumValue: 200;
                maximumValue: 400;
                value: minimumValue;
                stepSize: 50;

                style: SliderStyle {
                    handle: Item {
                        height: 14;
                        width: height;

                        Rectangle {
                            id: handleRectangle;
                            anchors.fill: parent;
                            radius: width / 2;
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "#5f6273"; }
                                GradientStop { position: 1.0; color: "#3a3d46"; }
                            }

                            DropShadow {
                                source: innerHandleRectangle;
                                anchors.fill: source;
                                verticalOffset: 0;
                                horizontalOffset: 0;
                                color: "red";
                                transparentBorder: true;
                                radius: 16;
                                samples: radius * 2;
                            }

                            Rectangle {
                                id: innerHandleRectangle;
                                anchors {
                                    centerIn: parent;
                                }
                                height: 6;
                                width: height;
                                radius: height / 2;
                                gradient: Gradient {
                                    GradientStop { position: 0.0; color: "#ca728a"; }
                                    GradientStop { position: 1.0; color: "#de8a8a"; }
                                }

                            }
                        }
                    }

                    groove: Item {
                        width: control.width;
                        height: control.height * 0.2;

                        Rectangle {
                            anchors.fill: parent;
                            color: "#1a1b20";
                            radius: 3;
                            border {
                                width: 1;
                                color: "#111216";
                            }

                            Rectangle {
                                z: parent.z - 1;
                                radius: parent.radius;
                                anchors {
                                    fill: parent;
                                    bottomMargin: -1;
                                }
                                color: "white";
                                opacity: 0.1;

                            }
                        }
                    }
                }
            }

            Rectangle {
                color: "transparent";
                border {
                    width: 3;
                    color: "white";
                }

                height: 10;
                width: height;

                anchors {
                    verticalCenter: parent.verticalCenter;
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

        anchors {
            fill: parent;
        }

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

            color: "transparent";

            objectName: "BoxartGridView";

        }
    }

    Component {
        id: detailGameViewComponent;


        DetailedGameView {

        }
    }

    Component {
        id: inputView;

        InputView {

            objectName: "InputView"

        }
    }


}
