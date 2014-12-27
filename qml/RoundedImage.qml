import QtQuick 2.3
import QtGraphicalEffects 1.0
import phoenix.image 1.0

Item {
    id: roundedImage;

    property real aspectRatio: 1.0;
    property url source: gridItem.imageSource;

    MouseArea {
        id: mouseArea;
        propagateComposedEvents: true;
        anchors.fill: parent;
        hoverEnabled: true;
        enabled: true;
        property bool containsMouse: false;
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onPressed:  {

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


    Image {
        id: sourceImage
        source: parent.source;
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent;
        visible: false
        asynchronous: true;
        onPaintedHeightChanged:  {
            gridItem.paintedHeight = paintedHeight;
            roundedImage.aspectRatio = paintedWidth / paintedHeight
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

        CachedImage {
            id: cachedImage;
            imgsrc: sourceImage.source;
            folder: "Artwork";
            fileName: gridItem.titleName ? gridItem.titleName : "";
            cacheDirectory: root.cacheDirectory;

            onLocalsrcChanged: {
                sourceImage.source = localsrc;
            }
        }

        Image {
            id:  border;
            source: "../assets/border-highlight.png"
            anchors.centerIn: parent;
            height: parent.height;
            width: parent.width
            fillMode: Image.Stretch;
            clip: true;
            asynchronous: true;

        }
    }

    Rectangle {
        id: mask
        anchors { fill: parent; margins: 18 }
        color: "black";
        radius: 4
        clip: true
        visible: false
    }

    OpacityMask {
        anchors.fill: parent;
        source: sourceImage;
        maskSource: mask
    }
}
