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

    Rectangle {
        id: descriptiveArea;
        color: "#212121";
        height: expanded ? 175 : 0;
        property bool expanded: false;

        anchors {
            right: parent.right;
            top: parent.top;
            left: parent.left;
        }

        Behavior on height {
            PropertyAnimation {}
        }

        Rectangle {
            color: "#1a1a1a";
            anchors {
                fill: parent;
                leftMargin: 1;
                rightMargin: 1;
                bottomMargin: 2;
                topMargin: 1;
            }

            Column {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    margins: 15;
                }

                Text {
                    text: gridView.currentItem.titleName;
                    color: "#f1f1f1";
                    renderType: Text.QtRendering;
                    font {
                        pixelSize: 16;
                        bold: true;
                        family: "Sans";
                    }
                }

                Text {
                    text: gridView.currentItem.systemName;
                    color: "gray";
                    renderType: Text.QtRendering;
                    font {
                        pixelSize: 14;
                        family: "Sans";
                    }
                }
            }

            Image {
                opacity: 0.3;
                anchors {
                    right: parent.right;
                    top: parent.top;
                    topMargin: 12;
                    rightMargin: 12;
                }

                source: gridView.currentItem.imageSource;
                height: 125;
                width: 125;
            }


            Row {
                anchors {
                    bottom: parent.bottom;
                    horizontalCenter: parent.horizontalCenter;
                    bottomMargin: 5;
                }
                spacing: 5;

                PhoenixNormalButton {
                    text: "Play";
                    onClicked: {
                        //windowStack.push({item: gameView, properties: {coreName: "", gameName: "", run: true}});
                    }
                }

                PhoenixWarningButton {
                    text: "Remove Game";
                    onClicked:  {
                        phoenixLibrary.deleteRow(gridView.currentIndex);
                        descriptiveArea.expanded = false;
                    }
                }
            }
        }

        Rectangle {
            id: botBorder;
            height: 1;
            color: "#333333";
            anchors {
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
            }
        }
    }

    PhoenixScrollView {
        id: scrollView;
        anchors {
            top: descriptiveArea.bottom;
            left: parent.left;
            bottom: parent.bottom;
            right: parent.right;
        }

        MouseArea {
            id: rootMouse;
            anchors.fill: parent;
            enabled: gridView.holdItem;
            propagateComposedEvents: false;
            hoverEnabled: true;
            onClicked:  {
                gridView.holdItem = false;
                descriptiveArea.expanded = false;
                gridView.holdItem = false;
                if (gridView.currentItem)
                    gridView.currentItem.glowColor = "black";
            }
        }

    GridView {
        id: gridView;

        property bool checked: false;
        property bool holdItem: false;

        snapMode: GridView.NoSnap;

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
            height: gridView.cellHeight - (40 * gameGrid.zoomFactor);
            width: gridView.cellWidth; //- (10 *  gameGrid.zoomFactor);

            property string glowColor: "black";
            property string imageSource: !artwork ? "qrc:/assets/No-Art.png" : artwork;
            property string titleName: title ? title : "";
            property string systemName: "Current System";

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
                        visible: true;
                        height: image.paintedHeight;
                        width: image.paintedWidth;
                        anchors.centerIn: parent;
                        glowRadius: 10//mouseArea.containsMouse ? 5 : 10;
                        spread: mouseArea.containsMouse ? 0.3 : 0.2;
                        color:  gridItem.glowColor;
                    }
                    Image {
                        id: image;
                        anchors.fill: parent;
                        anchors.margins: 10;
                        source: gridItem.imageSource;
                        fillMode: Image.PreserveAspectFit;

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
                            enabled: !rootMouse.enabled;
                            property bool containsMouse: false;

                            onPressed:  {
                                gridView.holdItem = pressed;
                                descriptiveArea.expanded = pressed;
                                containsMouse = pressed;
                            }
                            onClicked: {
                                gridView.currentIndex = index;
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
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                        bottomMargin: -titleLabel.font.pixelSize;
                    }

                    text: gridItem.titleName;
                    color: "#f1f1f1";

                    font {
                        bold: true;
                        pixelSize: 3 + gridView.cellWidth / 20;
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
