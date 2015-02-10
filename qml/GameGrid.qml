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
        anchors {
            fill: parent;
        }

    GridView {
        id: gridView;

        property bool checked: false;
        property bool holdItem: false;

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

        states: [
            State {
                name: "resizing";
                when: gameGrid.resizeGrid;
                PropertyChanges {
                    target: gridView;
                    cellHeight: 100 * gameGrid.zoomFactor;
                    cellWidth: 100 * gameGrid.zoomFactor;
                }
            }

        ]

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
            topMargin: 40;
            bottomMargin: 40;
        }

        cellWidth: 300;
        cellHeight: 300;

        model: phoenixLibrary.model();
        highlightFollowsCurrentItem:true;

        highlight: Item {
            id: highlighter;
            //anchors.fill: gridView.currentItem;
            visible: !gridView.currentItem.imageLoaded;
            property bool isEvenWidth: gridView.currentItem.paintedWidth % 2 == 0;

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
                anchors {
                    left: parent.right;
                    leftMargin: -10 * gameGrid.zoomFactor;
                    verticalCenter: parent.verticalCenter;
                    verticalCenterOffset: 65 / gameGrid.zoomFactor;
                }
                z: gridView.currentItem.z;
                visible: gridView.currentItem.showMenu;
                radius: 2;
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



        delegate: Item {
            id: gridItem;
            height: gridView.cellHeight - (50 * gameGrid.zoomFactor);
            width: gridView.cellWidth; //- (10 *  gameGrid.zoomFactor);
            z: 0;
            property bool imageLoaded: false;
            property string imageSource: !artwork ? "qrc:/assets/No-Art.png" : artwork;
            property string titleName: title ? title : "";
            property string systemName: system ? system : "";
            property string fileName: filename ? filename : "";
            property string systemPathName: system_path ? system_path : "";
            property bool showMenu: false;
            property int paintedWidth: width;
            property int paintedHeight: height;



            Item {
                id: subItem;
                anchors.fill: parent;

                Item  {
                    id: imageHighlight;
                    anchors.fill: parent;

                    property bool imageVisible: image.visible && image.status === Image.Ready;

                    RectangularGlow {
                        visible: index !== gridView.currentIndex && parent.imageVisible;
                        anchors.centerIn: image;
                        width: image.paintedWidth;
                        height: image.paintedHeight;
                        glowRadius: 15;
                        spread: 0.1;
                        color: "#40000000";
                        cornerRadius: 2;
                    }


                    Rectangle {
                        radius: 3;
                        anchors {
                            fill: borderImage;
                            margins: -1;
                        }
                        visible: parent.imageVisible;
                        onHeightChanged: gridItem.paintedHeight = height;
                        onWidthChanged: gridItem.paintedWidth = width;
                        z: borderImage.z - 1;
                        color: "black";
                    }

                    BorderImage {
                        z: image.z + 1;
                        id: borderImage;
                        visible: parent.imageVisible;
                        source: "../assets/glow-mask.png"
                        width: image.height * image.aspectRatio;
                        height: image.height;
                        anchors.centerIn: image;
                        verticalTileMode: BorderImage.Stretch;
                        horizontalTileMode: BorderImage.Stretch;
                    }

                    Image {
                        visible: true;
                        property real aspectRatio: paintedWidth / paintedHeight;

                        id: image;
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                            horizontalCenter: parent.horizontalCenter;
                        }

                        source: gridItem.imageSource;
                        width: 201 //* aspectRatio;
                        fillMode: Image.PreserveAspectFit;
                        asynchronous: true;

                        sourceSize {
                            height: 201;
                            width: 201;
                        }


                        //onPaintedHeightChanged: console.log(paintedWidth / 2 - subItem.x)

                        onStatusChanged: {
                            if (status == Image.Ready)
                                gridItem.imageLoaded = false;
                            else
                                gridItem.imageLoaded = true;
                        }

                        Component.onCompleted: {
                            cachedImage.start();
                        }

                        CachedImage {
                            id: cachedImage;
                            imgsrc: image.source;
                            folder: "Artwork";
                            fileName: gridItem.titleName ? gridItem.titleName : "";
                            cacheDirectory: root.cacheDirectory;

                            onLocalsrcChanged: {
                                image.source = localsrc;
                            }
                        }

                        MouseArea {
                            id: mouseArea;
                            propagateComposedEvents: true;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            enabled: true;
                            property bool containsMouse: false;
                            acceptedButtons: Qt.LeftButton | Qt.RightButton

                            onPressed:  {
                                if (gridView.currentItem.showMenu)
                                    gridView.currentItem.showMenu = false;
                                gridView.currentIndex = index;
                                gridView.holdItem = pressed;
                                containsMouse = pressed;

                            }

                            onDoubleClicked: {
                                var curItem = gridView.currentItem;
                                var core = phoenixLibrary.getSystem(curItem.systemName);
                                root.gameAndCoreCheck(curItem.titleName, curItem.systemName, curItem.fileName, core);
                                root.lastGameName = title;
                                root.lastSystemName = system;
                            }

                            onClicked: {
                                gridView.currentItem.z = 0;
                                gridView.currentItem.showMenu = false;
                                gridView.currentIndex = index;
                                gridView.currentItem.z = 100;

                                if (mouse.button == Qt.RightButton) {
                                    if (gridView.currentItem.showMenu)
                                        gridView.currentItem.showMenu = false;
                                    else
                                        gridView.currentItem.showMenu = true;
                                }

                                if (windowStack.currentItem.run)
                                    headerBar.userText = gridItem.titleName;
                            }
                        }
                    }
                }

                Column {
                    spacing: 2;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        //leftMargin: 40;
                        top: imageHighlight.bottom;
                        topMargin: 24;
                    }
                    //height: 50;
                    width: 125;

                    Text {
                        id: titleLabel;
                        renderType: Text.QtRendering;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        text: gridItem.titleName;
                        color: "#f1f1f1";

                        font {
                            bold: true;
                            pixelSize: 11;
                            family: "Sans";
                        }

                        elide: Text.ElideRight;
                        horizontalAlignment: Text.AlignHCenter;
                    }

                    Text {
                        id: systemLabel;
                        visible: gameGrid.showSystem;
                        renderType: Text.QtRendering;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        text: gridItem.systemName;
                        color: "#f1f1f1";
                        font {
                            pixelSize: 10;
                            family: "Sans";
                        }

                        elide: Text.ElideRight;
                        horizontalAlignment: Text.AlignHCenter;
                    }
                }
            }
        }
    }
    }
}
