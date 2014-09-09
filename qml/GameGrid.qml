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

    GridView {
        id: gridView;

        property bool checked: false;

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


        ExclusiveGroup {
            id: gridGroup;
        }


        delegate: Item {
            id: gridItem;
            height: gridView.cellHeight - (50 / gameGrid.zoomFactor);
            width: gridView.cellWidth - (50 / gameGrid.zoomFactor);

            Item {
                anchors.fill: parent;

                Rectangle {
                    id: imageHighlight;

                    property ExclusiveGroup exclusiveGroup: gridGroup;
                    property bool checked: false

                    anchors {
                        centerIn: parent;
                    }

                    width: parent.width;
                    height: parent.height;
                    color: "#000000FF";

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
                        spread: 0.1//mouseArea.containsMouse ? 0.8 : 0.1;
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
                                //console.log('clicked: ' + );
                                if (imageHighlight.checked)
                                    imageHighlight.checked = false;
                                else
                                    imageHighlight.checked = true;
                                //windowStack.push({item: gameView, properties: {coreName: "", gameName: "", run: true}});
                                if (windowStack.currentItem.run)
                                    headerBar.userText = title;
                            }
                            onEntered: containsMouse = true;
                            onExited: containsMouse = false;
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
