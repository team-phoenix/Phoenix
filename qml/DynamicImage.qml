import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3;
import phoenix.image 1.0

Image {
    visible: true;

    property bool locallyCache: true;
    property real aspectRatio: paintedWidth / paintedHeight;

    id: image;
    width: gridItem.itemDeleted ? 0 : parent.width;

    source: gridItem.imageSource;
    fillMode: Image.PreserveAspectFit;
    asynchronous: true;
    property bool hovered: false;

    sourceSize {
        height: 500;
        width: 500;
    }



    Behavior on width {
        PropertyAnimation {duration: 50;  easing.type: Easing.Linear;}
    }

    onWidthChanged: {
        if (width === 0) {
            if (gridView.titleToDelete !== "")
                phoenixLibrary.deleteRow(gridView.titleToDelete);
        }
    }

    anchors {
        top: parent.top;
        bottom: parent.bottom;
        horizontalCenter: parent.horizontalCenter;
    }


    ProgressBar {
        z: 100;
        anchors {
            horizontalCenter: parent.horizontalCenter;
            bottom: parent.bottom;
            bottomMargin: 16;
        }

        visible: value !== 1.0;
        value: gridItem.imageProgress;
        minimumValue: 0.0;
        maximumValue: 1.0;
        height: 12;
        width: parent.paintedWidth / 2 + 1;

        style: ProgressBarStyle {
            background: Rectangle {
                radius: 2;
                height: control.height;
                width: control.width;
                color: "black";
                border {
                    width: 1;
                    color: "#4e4d4d";
                }
            }

            progress: Rectangle {
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#cdcbcb";}
                    GradientStop {position: 1.0; color: "#a1a1a1";}
                }
                border {
                    width: 1;
                    color: "#e6e4e4FF"
                }
            }
        }
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

    Component.onCompleted: {
        cachedImage.start();
    }

    MouseArea {
        id: mouseArea;
        propagateComposedEvents: true;
        anchors.fill: parent;
        hoverEnabled: true;
        enabled: true;
        property bool containsMouse: false;
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onEntered: parent.hovered = true;
        onExited: parent.hovered = false;

        onPressed:  {
            if (gridView.currentItem.showMenu)
                gridView.currentItem.showMenu = false;

            gridView.currentIndex = index;
            gridView.holdItem = pressed;
            containsMouse = pressed;

        }

        onDoubleClicked: {
            root.playGame(gridView.currentItem.titleName,
                     gridView.currentItem.systemName,
                     gridView.currentItem.fileName,
                     phoenixLibrary.getSystem(gridView.currentItem.systemName));
        }

        onClicked: {
            gridView.currentItem.showMenu = false;
            gridView.currentIndex = index;

            if (mouse.button == Qt.RightButton) {
                if (gridView.currentItem.showMenu)
                    gridView.currentItem.showMenu = false;
                else
                    gridView.currentItem.showMenu = true;
            }
        }
    }
}
