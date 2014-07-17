import QtQuick 2.2
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

Rectangle {
    id: gameGrid;
    color: "#1d1e1e";
    height: 500;
    width: 500;

    property string itemBackgroundColor: "#b85353";
    property real sliderValue: 0;
    property bool sliderPressed: false;

    GridView {
        id: gridView;

        property bool checked: false;

        snapMode: GridView.SnapToRow;


        anchors {
            fill: parent;
            leftMargin: 175;
            topMargin: 125;
            bottomMargin: 75;
        }
        cellWidth: 400 + sliderValue;
        cellHeight: 400 + sliderValue;

        model: gamelibrary;
        ExclusiveGroup {
            id: gridGroup;
        }


        delegate: Item {
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
                        MouseArea {
                            anchors.fill: parent;
                            onPressed: {
                                if (imageHighlight.checked)
                                    imageHighlight.checked = false;
                                else
                                    imageHighlight.checked = true;
                                gameView.run = true;
                                windowStack.push({item: gameView, replace: true });

                            }
                        }
                    }
                }

                DropShadow {
                    source: imageHighlight;
                    visible: !gameGrid.sliderPressed;
                    anchors.fill: source;
                    fast: false;
                    horizontalOffset: 3;
                    verticalOffset: 6;
                    radius: 8.0;
                    samples: 16;
                    color: "#80000000";
                    transparentBorder: true;
                }

                Label {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                        bottomMargin: 15;
                    }

                    text: title;
                    color: "#f1f1f1";

                    font {
                        bold: true;
                        pixelSize: 14;
                        family: "Sans";
                    }

                    elide: Text.ElideRight;
                    horizontalAlignment: Text.AlignHCenter;
                }
            }


            /*ColumnLayout {
            anchors.centerIn: parent;
            height: parent.height + sliderValue;
            width: parent.width + sliderValue;

            Rectangle {
                id: background;
                anchors.centerIn: parent;

                color: "red";
                height: 200 + sliderValue;
                width: 200 + sliderValue;

                property ExclusiveGroup exclusiveGroup: gridGroup;
                property bool checked: false

                onExclusiveGroupChanged: {
                    if (exclusiveGroup) {
                        exclusiveGroup.bindCheckable(background);
                    }
                }

                onCheckedChanged: {
                    if (checked)
                        color = gameGrid.itemBackgroundColor;
                    else
                        color = "#000000FF";
                }

                Rectangle {
                    color: "yellow";
                    anchors.fill: parent;
                    anchors.margins: 20;
                }


                Image {
                    source: artwork;
                    fillMode: Image.PreserveAspectFit;
                    anchors {
                        fill: parent;
                        rightMargin: 10;
                        leftMargin: 10;
                        topMargin: 2;
                        bottomMargin: 2;
                    }

                    onStatusChanged: {
                        if (status === Image.Ready) {
                            background.height = paintedHeight;
                            background.width = paintedWidth;
                        }

                    }

                    MouseArea {
                        anchors.fill: parent;
                        onPressed: background.checked = true;
                    }
                }
            }

            DropShadow {
                source: background;
                visible: !gameGrid.sliderPressed;
                anchors.fill: source;
                fast: false;
                horizontalOffset: 3;
                verticalOffset: 6;
                radius: 8.0;
                samples: 16;
                color: "#80000000";
                transparentBorder: true;
            }

            Item {
                anchors {
                    top: background.bottom;
                    topMargin: 20;
                }

                Label {
                    width: background.width;
                    font.bold: true;
                    font.pixelSize: 12;
                    color: "#f1f1f1";
                    text: title;
                    elide: Qt.ElideRight;
                    horizontalAlignment: Text.AlignHCenter;
                }
            }

        }*/
        }


    }
}
