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
    property alias contentInputView: inputView;

    Item {
        id: headerArea;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        height: 35;

        Row {
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
                rightMargin: 100;
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
                anchors {
                    verticalCenter: parent.verticalCenter;
                }

                width: 150;

                minimumValue: 200;
                maximumValue: 400;
                value: 200;
                stepSize: 50;

                style: SliderStyle {
                    handle: Item {
                        height: 13;
                        width: height;

                        DropShadow {
                            source: handleRectangle;
                            anchors.fill: source;
                            verticalOffset: 0;
                            horizontalOffset: 0;
                            color: "red";
                            transparentBorder: true;
                            radius: 12;
                            samples: radius * 2;
                        }

                        Rectangle {
                            id: handleRectangle;
                            anchors.fill: parent;
                            radius: width / 2;
                            color: "white";
                        }
                    }

                    groove: Rectangle {
                        width: control.width;
                        height: control.height * 0.3;
                        radius: 3;

                        Rectangle {
                            anchors {
                                top: parent.top;
                                left: parent.left;
                                bottom: parent.bottom;
                            }

                            color: "red";
                            width: styleData.handlePosition;

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

        Column {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
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

            color: "transparent" //"gray";

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
