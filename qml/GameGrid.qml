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
                    type: Easing.OutCubic;
                }
                //to: 100 * zoomFactor;
                duration: 250;
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
        highlightFollowsCurrentItem: false;


        ExclusiveGroup {
            id: gridGroup;
        }

        delegate: Item {
            id: gridItem;
            height: gridView.cellHeight - (50 * gameGrid.zoomFactor);
            width: gridView.cellWidth; //- (10 *  gameGrid.zoomFactor);
            z: 0;

            property string glowColor: "black";
            property string imageSource: !artwork ? "qrc:/assets/No-Art.png" : artwork;
            property string titleName: title ? title : "";
            property string systemName: system ? system : "";
            property string fileName: filename ? filename : "";
            property string systemPathName: system_path ? system_path : "";
            property bool showMenu: false;

            Item {
                id: subItem;
                anchors.fill: parent;

                Item  {
                    id: imageHighlight;

                    property ExclusiveGroup exclusiveGroup: gridGroup;
                    property bool checked: false

                    anchors {
                        centerIn: parent;
                    }

                    width: parent.width;
                    height: parent.height;


                    RectangularGlow {
                        id: rectangularGlow;
                        visible: width !== 0 && height !== 0;
                        height: image.paintedHeight;
                        width: image.paintedWidth;
                        anchors.centerIn: parent;
                        glowRadius: 6;
                        spread: mouseArea.containsMouse ? 0.2 : 0.1;
                        color:  gridItem.glowColor;
                        cornerRadius: 3;
                    }
                    Image {
                        id: image;
                        z: 0;
                        anchors.fill: parent;
                        anchors.margins: 10;
                        source: gridItem.imageSource;
                        fillMode: Image.PreserveAspectFit;
                        asynchronous: true;
                        sourceSize {
                            height: 275;
                            width: 275;
                        }

                        RightClickMenu {
                            id: dropdownMenu;
                            width: 100;
                            height: 200;
                            color: "red";
                            radius: 3;
                            z: gridItem.z;
                            visible: gridItem.showMenu;
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

                        CachedImage {
                            id: cachedImage;
                            imgsrc: image.source;
                            folder: "Artwork";
                            fileName: gridItem.titleName ? gridItem.titleName : "";
                            onLocalsrcChanged: {
                                image.source = localsrc;
                            }
                        }

                        Component.onCompleted: cachedImage.start();

                        MouseArea {
                            id: mouseArea;
                            propagateComposedEvents: true;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            enabled: true;
                            property bool containsMouse: false;
                            acceptedButtons: Qt.LeftButton | Qt.RightButton

                            onPressed:  {
                                gridView.holdItem = pressed;
                                containsMouse = pressed;

                            }

                            onDoubleClicked: {
                                var curItem = gridView.currentItem;
                                var core = phoenixLibrary.getSystem(curItem.systemName);
                                root.gameAndCoreCheck(curItem.titleName, curItem.systemName, curItem.fileName, core);
                            }

                            onClicked: {
                                gridView.currentItem.z = 0;
                                gridView.currentItem.showMenu = false;
                                gridView.currentItem.glowColor = "black";
                                gridView.currentIndex = index;
                                gridView.currentItem.z = 100;

                                if (mouse.button == Qt.RightButton) {
                                    if (gridView.currentItem.showMenu)
                                        gridView.currentItem.showMenu = false;
                                    else
                                        gridView.currentItem.showMenu = true;
                                }

                                if (gridView.currentItem.glowColor === "#db5753")
                                    gridView.currentItem.glowColor = "black";
                                else {
                                    gridView.currentItem.glowColor = "#db5753"
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
                        pixelSize: 2 + gridView.cellWidth / 23;
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
