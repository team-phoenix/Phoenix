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

    onZoomFactorChanged: {
        if (gridView.cellHeight * zoomFactor !== gridView.cellHeight)
            resizeGrid = true;
    }

    PhoenixScrollView {
        anchors.fill: parent;

        MouseArea {
            id: rootMouse;
            anchors.fill: parent;
            propagateComposedEvents: true;
            hoverEnabled: true;
            onMouseYChanged:  {

                console.log("mousey: " + mouseY + "  itemheight: " + gridView.currentItem.height);
            }
        }

    GridView {
        id: gridView;

        property bool checked: false;

        snapMode: GridView.NoSnap;

        Rectangle {
            id: hoverMenu;
            visible: gridView.checked;
            color: "yellow";
            height: 250;
            width: 175;
            property bool leftAlign: false;
            property bool topAlign: true;
            property int xLocation: 10 + gridView.currentItem.width + gridView.currentItem.x;
            property int negativeXLocation: -30 - gridView.currentItem.width + gridView.currentItem.x;
            x: {
                if  (xLocation < gridView.width - gridView.currentItem.width) {
                    leftAlign = false;
                    return xLocation;
                }
                leftAlign = true;
                return negativeXLocation;
            }

            y: {
                if (gridView.currentItem.y < gridView.height - hoverMenu.height) {
                    topAlign = true;
                    return gridView.currentItem.y;
                }
                topAlign = false;
                return (rootMouse.mouseY - gridView.currentItem.height) * 0.6;
            }

            /*onLeftAlignChanged: {
                if (leftAlign) {
                    triangle.anchors.left = undefined;
                    triangle.anchors.right = hoverMenu.anchors.right;
                }
                else {
                    triangle.anchors.left  = hoverMenu.anchors.left;
                    riangle.anchors.right = undefined;
                }
            }*/

            Rectangle {
                id: triangle;
                color: "red";
                height: 24;
                width: 24;
                rotation: 45;
                x: !hoverMenu.leftAlign ? 0 - (width / 2) : hoverMenu.width  - (width / 2);

                // Anchors didn't prove to work here, even with passing in
                // undefined for the anchor. Could be a bug.
                anchors {
                    top: parent.top;
                    topMargin: 15;
                }
            }
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
            height: gridView.cellHeight - (40 * gameGrid.zoomFactor);
            width: gridView.cellWidth; //- (10 *  gameGrid.zoomFactor);
            Item {
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
                    //color: "#000000FF";

                    onExclusiveGroupChanged: {
                        if (exclusiveGroup) {
                            exclusiveGroup.bindCheckable(imageHighlight);
                        }
                    }

                    RectangularGlow {
                        id: rectangularGlow;
                        visible: true;
                        height: image.paintedHeight;
                        width: image.paintedWidth;
                        anchors.centerIn: parent;
                        glowRadius: 10//mouseArea.containsMouse ? 5 : 10;
                        spread: mouseArea.containsMouse ? 0.3 : 0.2;
                        color: mouseArea.containsMouse ? "#db5753" : "black";
                    }

                    Image {
                        id: image;
                        anchors.fill: parent;
                        anchors.margins: 10;
                        source: !artwork ? "qrc:/assets/No-Art.png" : artwork;
                        fillMode: Image.PreserveAspectFit;

                        CachedImage {
                            id: cachedImage;
                            imgsrc: image.source;
                            folder: "Artwork";
                            fileName: title;
                            onLocalsrcChanged: {
                                //console.log(localsrc);
                                image.source = localsrc;
                            }
                        }

                        Component.onCompleted: cachedImage.start();

                        MouseArea {
                            id: mouseArea;
                            propagateComposedEvents: true;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            property bool containsMouse: false;
                            onClicked: {
                                if (imageHighlight.checked)
                                    imageHighlight.checked = false;
                                else
                                    imageHighlight.checked = true;
                                //windowStack.push({item: gameView, properties: {coreName: "", gameName: "", run: true}});
                                if (windowStack.currentItem.run)
                                    headerBar.userText = title;
                            }
                            onEntered: {
                                containsMouse = true;
                                gridView.checked = true;
                                gridView.currentIndex = index;
                            }
                            onExited:  {
                                containsMouse = false;
                                gridView.checked = false;
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

                    text: title;
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
