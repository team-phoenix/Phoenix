import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

Rectangle {
    id: gameGrid;
    color: "#262626";
    height: 500;
    width: 500;

    property string itemBackgroundColor: "red";
    property bool showSystem: true;
    property real zoomFactor: 1;
    property bool zoomSliderPressed: false;
    property bool resizeGrid: false;


    Rectangle {
        // bottomBorder;
        z: gridView.z + 1;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "#0b0b0b";
    }

    Component.onCompleted: {
        root.itemInView = "grid";
    }

    onZoomFactorChanged: {
        if (gridView.cellHeight * zoomFactor !== gridView.cellHeight)
            resizeGrid = true;
    }



    PhoenixScrollView {
        id: scrollView;
        focus: root.gridFocus;
        onFocusChanged: {
            //console.log(focus ? "grid has focus" : "grid lost focus");
            //if (focus)
                //scrollView.forceActiveFocus();
        }

        anchors {
            fill: parent;
        }

        Keys.forwardTo: headerBar.textField;
        Keys.priority: Keys.BeforeItem;
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Right:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexRight();
                    event.accepted = true;
                    break;
                case Qt.Key_Left:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexLeft();
                    event.accepted = true;
                    break;
                case Qt.Key_Up:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection =  gridView.moveCurrentIndexUp();
                    event.accepted = true;
                    break;
                case Qt.Key_Down:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexDown();
                    event.accepted = true;
                    break;

                default:
                    //headerBar.textField.carriedEvent = String.fromCharCode(event.key);
                    //headerBar.textField.focus = true;

                    break;
            }

        }

        GridView {
            id: gridView;

            property bool checked: false;
            property bool indexUpdated: false;
            property bool animateHighlighter: false;
            property int cacheLimit: 6;
            property int cacheValue: 0;
            property string titleToDelete: "";
            property bool shrink: false;
            property int queuedIndex: 0;
            property int cacheIndex: 0;
            property var moveCurrentDirection: -1;

            //cacheBuffer: 10;
            onCurrentIndexChanged: animateHighlighter = true;

            Text {
                visible: gridView.count == 0;
                anchors.centerIn: parent;
                text: "0 Games Were Found, (PLACE HOLDER)";
                color: "#f1f1f1";
                font {
                    family: "Sans";
                    pixelSize: 32;
                }
            }

            Behavior on cellHeight {
                PropertyAnimation {
                    //id: resizeAnimation;
                   // target: gridView;
                    //properties: "cellWidth, cellHeight";
                    easing {
                        type: Easing.Linear;
                    }
                    //to: 100 * zoomFactor;
                    duration: 50;
                }
            }

            anchors {
                fill: parent;
                leftMargin: (parent.width >= cellWidth) ? ((parent.width % cellWidth) / 2) : 0;
                rightMargin: leftMargin;
                bottomMargin: 40;
            }

            cellHeight: 100 * gameGrid.zoomFactor;
            cellWidth: 100 * gameGrid.zoomFactor;
            model: phoenixLibrary.model();
            highlightFollowsCurrentItem: false;

            currentIndex: 0;
            property bool showRightClickMenu: false;


            highlight: Item {
                id: highlighter;
                property bool isEvenWidth: gridView.currentItem.paintedWidth % 2 == 0;
                z: 1;
                visible: root.gridFocus;
                //y: (gridView.currentItem.height + gridView.currentItem.y) / 2;
                //x: (gridView.currentItem.x + gridView.currentItem.width) / 2;
                height: gridView.currentItem.height;
                width: gridView.currentItem.width;
                y: gridView.currentItem.y;
                x: gridView.currentItem.x;

                RectangularGlow {
                    id: effect;
                    anchors.fill: highlightBorder;
                    visible: realHighlighter.visible;
                    glowRadius: 12;
                    spread: 0.1;
                    color: "#ec302e";
                    cornerRadius: realHighlighter.radius + glowRadius * 2;
                }

                BorderImage {
                    id: highlightBorder;
                    source: "../assets/border-highlight.png";
                    visible: realHighlighter.visible
                    anchors {
                        fill: realHighlighter;
                    }

                    z: realHighlighter.z + 1;
                    verticalTileMode: BorderImage.Stretch;
                    horizontalTileMode: BorderImage.Stretch;
                }

                Rectangle {
                    id: realHighlighter;
                    radius: 3;
                    width: gridView.currentItem.artworkItemAlias.artworkImage.paintedWidth + 15;
                    height: gridView.currentItem.artworkItemAlias.artworkImage.paintedHeight + 15;
                    anchors {
                        centerIn: highlighter;
                    }

                    z: parent.z + 2;

                    Component.onCompleted: state = "grow";

                    states: [
                       State {
                            name: "shrink";
                            when: gridView.shrink;
                            PropertyChanges {
                                id: shrinkAnimation;
                                target: realHighlighter;
                                height: 0;
                                width: 0;
                                y: (gridView.currentItem.height + gridView.currentItem.y) / 2;
                                x: (gridView.currentItem.x + gridView.currentItem.width) / 2;
                            }
                        },
                        State {
                            id: grow;
                            name: "grow";
                            PropertyChanges {
                                id: growAnimation;
                                target: realHighlighter;

                                width: gridView.currentItem.artworkItemAlias.artworkImage.paintedWidth + 15;
                                height: gridView.currentItem.artworkItemAlias.artworkImage.paintedHeight + 15;

                            }
                        }

                    ]

                    transitions: [
                        Transition {
                            from: "grow";
                            to: "shrink";

                            NumberAnimation {
                                properties: "width,height,x,y";
                                easing.type: Easing.InBack;
                                duration: 150;
                            }

                            onRunningChanged: {
                                if (!running) {
                                    realHighlighter.state = "grow";
                                    if (gridView.moveCurrentDirection !== -1) {
                                        gridView.moveCurrentDirection;
                                    }
                                    else
                                        gridView.currentIndex = gridView.queuedIndex;

                                }

                            }
                        },
                        Transition {
                            from: "shrink";
                            to: "grow";
                            NumberAnimation {
                                properties: "width,height,x,y";
                                easing.type: Easing.OutBack;
                                duration: 150;
                            }

                            onRunningChanged: {
                                if (!running) {
                                    gridView.shrink = false;
                                    gridView.moveCurrentDirection = -1;
                                }
                            }

                        }
                    ]


                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "#ff730f";}
                        GradientStop {position: 1.0; color: "#e9163b";}
                    }

                    Rectangle {
                        anchors {
                            centerIn: parent;
                            //verticalCenterOffset: 1;
                            //horizontalCenterOffset: 0;
                        }

                        height: gridView.currentItem.artworkItemAlias.artworkImage.paintedHeight + 2;
                        width: gridView.currentItem.artworkItemAlias.artworkImage.paintedWidth + 2;
                        color: "white";
                        opacity: 0.35;
                        radius: 2;

                        /*CustomBorder {
                            color: "white";
                            opacity: 0.85;
                            radius: 2;
                        }*/
                    }

                }

                RectangularGlow {
                    visible: dropdownMenu.visible;
                    anchors.fill: dropdownMenu;
                    glowRadius: 3;
                    spread: 0.3;
                    color: "#f0000000";
                    cornerRadius: dropdownMenu.radius + glowRadius;
                }

                Rectangle {
                    color: "#4e4e4e";
                    visible: gridView.showRightClickMenu;
                    id: dropdownMenu;
                    width: 100;
                    height: 200;
                    x: realHighlighter.x + realHighlighter.width + 12;
                    anchors {
                        //left: parent.right;
                        //leftMargin: -10 * gameGrid.zoomFactor;
                        verticalCenter: parent.verticalCenter;
                        verticalCenterOffset: 65 / gameGrid.zoomFactor;
                    }
                    radius: 2;
                    z: 2;


                    RightClickMenu {
                        id: rightClickMenu;
                        anchors {
                            fill: parent;
                            margins: 1;
                        }

                        color: "#141414";
                        radius: parent.radius;
                    }
                }
            }


            //Connections {
            //    target: phoenixLibrary;
            //    onRowAdded: console.log("Game Added.");
            //   onRowDeleted: console.log(titleToDelete + " was deleted.");
            //}





            delegate: Item {
                id: gridItem;
                height: gridView.cellHeight;
                width: gridView.cellWidth;
                z: index == gridView.currentIndex ? 2 : 0;
                property bool imageLoaded: false;
                property int paintedWidth: width;
                property int paintedHeight: height;
                property real imageProgress: 0.0;
                property bool itemDeleted: false;
                property alias artworkItemAlias: artworkItem;
                property bool startCache: false;

                GridImage  {
                    id: artworkItem;
                    anchors {
                        fill: parent;
                        leftMargin: 10;
                        rightMargin: 10;
                        topMargin: 30;
                        bottomMargin: 30;
                   }
                }
            }
        }
    }
}
