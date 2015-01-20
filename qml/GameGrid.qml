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

        MouseArea {
            id: gridBackgroundMouse;
            anchors.fill: parent;
            enabled: false;
            propagateComposedEvents: true;
            onClicked: gridView.currentItem.showMenu = false;
        }

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
            anchors.fill: gridView.currentItem;
            visible: !gridView.currentItem.imageLoaded;
            property bool isEvenWidth: gridView.currentItem.paintedWidth % 2 == 0;


            RectangularGlow {
                id: effect;
                anchors.fill: realHighlighter;
                visible: realHighlighter.visible;
                glowRadius: 12;
                spread: 0.1;
                color: "#ec302e";
                cornerRadius: realHighlighter.radius + glowRadius * 2;
            }

            Rectangle {
                id: realHighlighter;
                radius: 2;
                width: gridView.currentItem.paintedWidth + 20;
                height: gridView.currentItem.paintedHeight + 20;
                anchors {
                    centerIn: parent;
                    horizontalCenterOffset: highlighter.isEvenWidth ? 0 : 1;
                }

                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#ff730f";}
                    GradientStop {position: 1.0; color: "#e9163b";}
                }

                Rectangle {
                    anchors {
                        centerIn: parent;
                        verticalCenterOffset: 1;
                        horizontalCenterOffset: 0;
                    }

                    height: parent.height - 15;
                    width: parent.width - 16;
                    color: "#0f0f0f";
                    opacity: 0.40;
                    radius: 2;

                    CustomBorder {
                        color: "white";
                        opacity: 0.75;
                        radius: 2;
                    }
                }


                CustomBorder {

                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "#fea266";}
                        GradientStop {position: 1.0; color: "#ed5871";}
                    }

                    CustomBorder {
                        color: "#0f0f0f";
                    }
                }

            }

            RectangularGlow {
                visible: dropdownMenu.visible;
                anchors.fill: dropdownMenu;
                glowRadius: 10;
                spread: 0.1;
                color: "#d0000000";
                cornerRadius: dropdownMenu.radius + glowRadius;
            }

            RightClickMenu {
                id: dropdownMenu;
                width: 100;
                height: 200;
                color: "#1e1e1e";
                radius: 3;
                z: gridView.currentItem.z;
                visible: gridView.currentItem.showMenu;
                anchors {
                    left: parent.right;
                    leftMargin: -10 * gameGrid.zoomFactor;
                    verticalCenter: parent.verticalCenter;
                    verticalCenterOffset: 65 / gameGrid.zoomFactor;
                }
                onVisibleChanged: {
                    gridBackgroundMouse.enabled = visible;
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

                    anchors {
                        centerIn: parent;
                    }

                    width: parent.width;
                    height: parent.height;

                    /*InnerShadow {
                        source: image;
                        anchors.fill: source;
                        horizontalOffset: 0;
                        verticalOffset: 0;
                        color: "#b0000000";
                        radius: 4.0;
                        samples: radius * 2;
                        spread: 1.0;
                    }*/

                    RectangularGlow {
                        visible: index !== gridView.currentIndex && image.visible && image.status === Image.Ready;
                        anchors.centerIn: image;
                        width: image.paintedWidth;
                        height: image.paintedHeight;
                        glowRadius: 15;
                        spread: 0.1;
                        color: "#40000000";
                        cornerRadius: 2;
                    }


                    Image {
                        //visible: false;
                        id: image;
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                            horizontalCenter: parent.horizontalCenter;
                        }

                        source: gridItem.imageSource;
                        width: 100 //* aspectRatio;
                        fillMode: Image.PreserveAspectFit;
                        asynchronous: true;
                        onPaintedHeightChanged:  {
                            gridItem.paintedHeight = paintedHeight;
                        }
                        onPaintedWidthChanged: gridItem.paintedWidth = paintedWidth;


                        sourceSize {
                            height: 200;
                            width: 200;
                        }

                        onStatusChanged: {
                            if (status == Image.Ready)
                                gridItem.imageLoaded = false;
                            else
                                gridItem.imageLoaded = true;
                        }

                        Component.onCompleted: {
                            cachedImage.start();
                        }



                        Rectangle {
                            id: topBorder;
                            y: leftBorder.y;
                            visible: parent.visible && parent.status === Image.Ready;
                            anchors {
                                left: leftBorder.right;
                                right: parent.right;
                                rightMargin: 1;
                            }
                            width: parent.paintedWidth;
                            opacity: 0.3;
                            color: "white";
                            height: 1;
                        }

                        Rectangle {
                            id: borderBorder;
                            y: leftBorder.y + leftBorder.height - 1;
                            visible: parent.visible && parent.status === Image.Ready;
                            anchors {
                                left: leftBorder.right;
                                right: parent.right;
                                rightMargin: 1;
                            }
                            width: parent.paintedWidth;
                            opacity: 0.3;
                            color: "white";
                            height: 1;
                        }

                        Rectangle {
                            id: leftBorder;
                            property bool adjustHeight: 8 % (parent.paintedHeight % 10)  === 0;
                            visible: parent.visible && parent.status === Image.Ready;
                            width: 1;
                            height: adjustHeight ? parent.paintedHeight : parent.paintedHeight + 1;
                            anchors {
                                left: parent.left;
                                verticalCenter: parent.verticalCenter;
                                verticalCenterOffset: adjustHeight ? 0 : 1;
                                //rightMargin: 1;
                            }
                            opacity: 0.3;
                            color: "white";
                        }

                        Rectangle {
                            id: rightBorder;
                            property bool adjustHeight: 8 % (parent.paintedHeight % 10)  === 0;
                            visible: parent.visible && parent.status === Image.Ready;
                            width: 1;
                            height: adjustHeight ? parent.paintedHeight : parent.paintedHeight + 1;
                            anchors {
                                right: parent.right;
                                verticalCenter: parent.verticalCenter;
                                verticalCenterOffset: adjustHeight ? 0 : 1;
                                //rightMargin: 1;
                            }
                            onXChanged: console.log(x)
                            onYChanged: console.log(y)
                            opacity: 0.3;
                            color: "white";
                        }

                        CustomBorder {
                            visible: parent.visible && parent.status === Image.Ready;
                            anchors {
                                fill: undefined;
                                centerIn: parent;
                                verticalCenterOffset: (parent.paintedHeight % 10)  === 4 ? 0 : 1;
                            }
                            property bool evenWidth: parent.paintedWidth % 2 === 0;
                            width: parent.paintedWidth + 2//evenWidth ? parent.paintedWidth + 1 : parent.paintedWidth + 3;
                            height: (parent.paintedHeight % 10)  === 4 ? parent.paintedHeight + 2 : parent.paintedHeight + 3;
                            color: "black";
                            radius: 0;
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
                                console.log(gridItem.titleName + image.paintedHeight)
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

                Text {
                    id: titleLabel;
                    renderType: Text.QtRendering;
                    anchors {
                        //horizontalCenter: parent.horizontalCenter;
                        left: parent.left;
                        leftMargin: 20;
                        //leftMargin: 50;
                        right: parent.right;
                        rightMargin: 20;
                        bottom: parent.bottom;
                        bottomMargin: -titleLabel.font.pixelSize * 2;
                    }

                    text: gridItem.titleName;
                    color: "#f1f1f1";

                    font {
                        bold: true;
                        pointSize: 9;
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
