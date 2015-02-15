import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1
import phoenix.image 1.0

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
                    gridView.moveCurrentIndexRight();
                    event.accepted = true;
                    break;
                case Qt.Key_Left:
                    gridView.moveCurrentIndexLeft();
                    event.accepted = true;
                    break;
                case Qt.Key_Up:
                    gridView.moveCurrentIndexUp();
                    event.accepted = true;
                    break;
                case Qt.Key_Down:
                    gridView.moveCurrentIndexDown();
                    event.accepted = true;
                    break;
                case Qt.Key_Tab:
                    console.log("TAB PRESSEEDDDD");
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
        property bool holdItem: false;

        //keyNavigationWraps: true;


        snapMode: GridView.NoSnap;

        /*MouseArea {
            id: gridBackgroundMouse;
            anchors.fill: parent;
            enabled: false;
            propagateComposedEvents: true;
            onClicked: {
                console.log("ada")
                gridView.currentItem.showMenu = false;
            }
        }*/


        /*states: [
            State {
                name: "resizing";
                when: gameGrid.resizeGrid;
                PropertyChanges {
                    target: gridView;
                    cellHeight: 100 * gameGrid.zoomFactor;
                    cellWidth: 100 * gameGrid.zoomFactor;
                }
            }

        ]*/

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

        cellHeight: 130 * gameGrid.zoomFactor;
        cellWidth: 100 * gameGrid.zoomFactor;
        model: phoenixLibrary.model();
        highlightFollowsCurrentItem: true;

        property int highlighterZValue: 1;
        property string titleToDelete: "";



        highlight: Item {
            id: highlighter;
            property bool isEvenWidth: gridView.currentItem.paintedWidth % 2 == 0;
            z: gridView.highlighterZValue;

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
                width: gridView.currentItem.paintedWidth + 21;
                height: gridView.currentItem.paintedHeight + 21;
                anchors {
                    centerIn: parent;
               }

                y: gridView.currentItem.artwork_1.y;
                x: gridView.currentItem.artwork_1.x;
                z: parent.z + 2;

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

                    height: gridView.currentItem.paintedHeight + 2;
                    width: gridView.currentItem.paintedWidth + 2;
                    color: "white";
                    opacity: 0.35;
                    radius: 2;

                    /*CustomBorder {
                        color: "white";
                        opacity: 0.85;
                        radius: 2;
                    }*/
                }


                /*CustomBorder {

                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "#fea266";}
                        GradientStop {position: 1.0; color: "#ed5871";}
                    }

                    CustomBorder {
                        color: "black";
                    }
                }*/

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
                visible: gridView.currentItem.showMenu;
                radius: 2;
                z: 2;
                //onVisibleChanged: {
                 //   gridBackgroundMouse.enabled = visible;
                //}

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


        Connections {
            target: phoenixLibrary;
            onRowAdded: console.log("Game Added.");
            onRowDeleted: console.log(titleToDelete + " was deleted.");
        }

        delegate: Item {
            id: gridItem;
            height: gridView.cellHeight;
            width: gridView.cellWidth;
            z: index == gridView.currentIndex ? 2 : 0;
            property bool imageLoaded: false;
            property string imageSource: !artwork ? "qrc:/assets/No-Art.png" : artwork;
            property string titleName: title ? title : "";
            property string systemName: system ? system : "";
            property string fileName: filename ? filename : "";
            property string systemPathName: system_path ? system_path : "";
            property bool showMenu: false;
            property int paintedWidth: width;
            property int paintedHeight: height;
            property real imageProgress: 0.0;
            property bool itemDeleted: false;
            property alias artwork_1: imageHighlight;

            GridImage  {
                id: imageHighlight;
                anchors {
                    fill: parent;
                    leftMargin: 50;
                    rightMargin: 50;
                    topMargin: 100;
                    bottomMargin: 100;
               }
            }
        }
    }
    }
}
