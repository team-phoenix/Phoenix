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

    border {
        width: 1;
        color: "#1a1a1a";
    }

    Rectangle {
        // bottomBorder;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: 2;
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
                        source: !artwork ? "qrc:/assets/No-Art.png" : artwork;
                        sourceSize {
                            height: 300;
                            width: 500;
                        }

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

/*
                        Button {
                            id: infoButton;
                            height: 48 - (30 / gameGrid.zoomFactor);
                            width: 48 - (30 / gameGrid.zoomFactor);
                            visible: false;
                            anchors {
                                right: parent.right;
                                top: parent.top;
                                rightMargin: 24;
                                topMargin: 24;
                            }

                            style: ButtonStyle {
                                label: Item {
                                    //height: 32;
                                    //width: 32;
                                    Image {
                                        id: infoIcon;
                                        source: "../assets/Info-32.png";
                                        anchors {
                                            fill: parent;
                                        }
                                    }
                                }

                                background: Rectangle {
                                    height: 32;
                                    width: 32;
                                    radius: 16;
                                    border {
                                        width: 1;
                                        color: "#262626";
                                    }

                                    gradient: Gradient {
                                        GradientStop {position: 0.0; color: "#2f2f2f";}
                                        GradientStop {position: 0.0; color: "#1d1d1e";}

                                    }
                                }

                            }
                        }


                        Rectangle {
                            id: infoDropDown;
                            z: gridView.z + 1;
                            visible: true;
                            color: "red";
                            anchors {
                                left: infoButton.right;
                                leftMargin: 25;
                                verticalCenter: infoButton.verticalCenter;
                                verticalCenterOffset: 125;
                            }

                            height: 300;
                            width: 200;
                            Rectangle {
                                z: parent.z + 1;
                                rotation: 45;
                                height: 25;
                                width: 25;
                                color: "red";
                                anchors {
                                    right: parent.left;
                                    rightMargin: -15;
                                    top: parent.top;
                                    topMargin: 25;
                                }
                            }
                        }
*/
                        Image {
                            source: "../assets/GameView/play.png"
                            visible: true;
                            sourceSize {
                                height: 64;
                                width: 64;
                            }
                            height: 40;
                            width: 40;
                            anchors {
                                fill: parent;
                                topMargin: 60;
                                leftMargin: 60;
                                rightMargin: 60;
                                bottomMargin: 5;
                                bottom: parent.bottom;

                            }
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: {
                                    //console.log('clicked: ' + );
                                    if (imageHighlight.checked)
                                        imageHighlight.checked = false;
                                    else
                                        imageHighlight.checked = true;
                                    if (gameView.run)
                                        headerBar.userText = title;
                                }
                            }
                        }

                        MouseArea {
                            id: mouseArea;
                            propagateComposedEvents: true;
                            anchors.fill: parent;
                            hoverEnabled: true;
                            //onEntered: infoButton.visible = true;
                            //onExited: infoButton.visible = false;
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
                        bottomMargin: titleLabel.font.pixelSize / 2;
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
