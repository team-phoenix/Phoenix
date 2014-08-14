import QtQuick 2.3
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

import phoenix.image 1.0


Rectangle {
    id: gameGrid;
    color: "#1d1e1e";
    height: 500;
    width: 500;

    property string itemBackgroundColor: "#b85353";
    property real zoomFactor: 1;
    property bool zoomSliderPressed: false;
    property bool resizeGrid: false;

    onZoomFactorChanged: {
        if (gridView.cellHeight * zoomFactor !== gridView.cellHeight)
            resizeGrid = true;
    }


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
            topMargin: 20;
            bottomMargin: 20;
        }

        cellWidth: 100;
        cellHeight: 100;

        model: gamelibrary;


        ExclusiveGroup {
            id: gridGroup;
        }


        delegate: Item {
            id: gridItem;
            height: gridView.cellHeight;
            width: gridView.cellWidth;


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
                    height: parent.height * 0.7;
                    color: checked ? gameGrid.itemBackgroundColor : "#000000FF";

                    onExclusiveGroupChanged: {
                        if (exclusiveGroup) {
                            exclusiveGroup.bindCheckable(imageHighlight);
                        }
                    }

                    Image {
                        id: image;
                        anchors.fill: parent;
                        anchors.margins: 10;
                        source: artwork;
                        sourceSize {
                            height: 200;
                            width: 200;
                        }
                        fillMode: Image.PreserveAspectFit;

                        CacheImage {
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
                            anchors.fill: parent;
                            onClicked: imagesaver.save(image, "C:/Users/lee/Desktop/test.jpg");
                            /*onPressed: {
                                if (imageHighlight.checked)
                                    imageHighlight.checked = false;
                                else
                                    imageHighlight.checked = true;
                                if (gameView.coreName == "")
                                    gameView.coreName = "C:/Users/lee/Desktop/32_cores/snes9x_libretro.dll"
                                if (gameView.gameName == "")
                                    gameView.gameName = "C:/Users/lee/Documents/Emulation/SNES/Super Mario All-Stars + Super Mario World (USA).sfc";
                                gameView.run = true;
                                gameView.loadSaveState = true;
                                windowStack.push({item: gameView, replace: true });

                            }*/
                        }
                    }
                }

                /*DropShadow {
                    source: imageHighlight;
                    visible: !gameGrid.zoomSliderPressed;
                    anchors.fill: source;
                    fast: false;
                    horizontalOffset: 3;
                    verticalOffset: 6;
                    radius: 8.0;
                    samples: 16;
                    color: "#80000000";
                    transparentBorder: true;
                }*/

                Label {
                    id: titleLabel;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                        bottomMargin: titleLabel.font.pixelSize / 2;
                    }

                    text: title;
                    color: "#f1f1f1";

                    font {
                        bold: true;
                        pixelSize: 6 + gridView.cellWidth / 20;
                        family: "Sans";
                    }

                    elide: Text.ElideRight;
                    horizontalAlignment: Text.AlignHCenter;
                }
            }
        }
    }
}
