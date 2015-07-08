import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Rectangle {
    id: boxartGridBackground;
    width: 100;
    height: 62;

    GridView {
        id: gridView;
        model: libraryModel;

        // The max height and width of the grid's cells. This can be tweaked
        // to change the default size of the boxart.
        property int maxCellHeight: 200;
        property bool clampEdges: parent.width >= maxCellHeight;

        displaced: Transition {
                NumberAnimation { properties: "x,y,visible,width,height,opacity"; duration: 1000 }
            }

        anchors {
            top: parent.top;
            topMargin: 24 * 3;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;

            leftMargin: gridView.clampEdges ? ( ( parent.width % cellWidth ) / 2 ) : 0;
            rightMargin: leftMargin;
            bottomMargin: 40;
        }

        // If the grid's width is less than the maxCellWidth, get
        // the grid to scale the size of the grid items, so that the transition looks really
        // seamless.
        cellHeight: clampEdges ? maxCellHeight : parent.width;
        cellWidth: cellHeight;

        boundsBehavior: Flickable.StopAtBounds;

        Component.onCompleted: libraryModel.updateCount();

        delegate: Rectangle {

            height: gridView.cellHeight//gridView.clampEdges ? gridView.cellHeight : gridView.width * 0.75;
            width: gridView.cellWidth;

            color: "lightgray"

            ColumnLayout {
                spacing: 6;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 24;
                    left: parent.left;
                    right: parent.right;
                    leftMargin: 24
                    rightMargin: 24;
                }

                Rectangle {
                    id: gridItemImageContainer;
                    color: "white";
                    Layout.fillHeight: true;
                    Layout.fillWidth: true;

                    DropShadow {
                        anchors.fill: source;
                        source: gridItemImage;
                        verticalOffset: 1;
                        horizontalOffset: 0;
                        color: "#a0000000";
                        transparentBorder: true;
                        samples: radius * 2;
                        radius: 6;
                    }

                    Image {
                        id: gridItemImage;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            bottom: parent.bottom;
                        }

                        onStatusChanged: {
                            if ( status == Image.Null || status === Image.Error) {
                                console.log("Error in " + source)
                            }
                        }

                        height: parent.height;


                        asynchronous: true;
                        source: artworkUrl;

                        verticalAlignment: Image.AlignBottom;

                        sourceSize {
                            height: 300;
                            width: 300;
                        }

                        fillMode: Image.PreserveAspectFit;

                    }
                }


                Text {
                    id: titleText;
                    text: rowIndex//title;
                    color: "black"//root.normalFontColor;
                    Layout.fillWidth: true;
                    elide: Text.ElideRight;
                }

                Text {
                    id: platformText;
                    anchors {
                        top: titleText.bottom;
                        topMargin: 0;
                    }

                    text: system;
                    color: "black"//root.normalFontColor;
                    Layout.fillWidth: true;
                    elide: Text.ElideRight;
                }

                Text {
                    id: absPath;
                    anchors {
                        top: platformText.bottom;

                    }
                    text: fileName;
                    color: "black"//root.normalFontColor;
                    Layout.fillWidth: true;
                    elide: Text.ElideMiddle;
                }
            }
        }
    }
}

