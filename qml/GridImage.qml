import QtQuick 2.4
import QtGraphicalEffects 1.0
import phoenix.image 1.0

Item  {
    id: imageHighlight;
    anchors.fill: parent;

    property bool imageVisible: image.visible && image.status === Image.Ready;

    RectangularGlow {
        visible: index !== gridView.currentIndex && parent.imageVisible;
        anchors.centerIn: image;
        width: image.paintedWidth;
        height: image.paintedHeight;
        glowRadius: 15;
        spread: 0.1;
        color: "#40000000";
        cornerRadius: 2;
    }


    Rectangle {
        id: blackBorder;
        radius: 3;
        anchors {
            fill: borderImage;
            margins: -1;
        }
        visible: parent.imageVisible;
        onHeightChanged: gridItem.paintedHeight = height;
        onWidthChanged: gridItem.paintedWidth = width;
        z: borderImage.z - 1;
        color: "black";
    }

    BorderImage {
        z: image.z + 1;
        id: borderImage;
        visible: parent.imageVisible;
        source: "../assets/glow-mask.png"
        width: image.height * image.aspectRatio;
        height: image.height;
        anchors.centerIn: image;
        verticalTileMode: BorderImage.Stretch;
        horizontalTileMode: BorderImage.Stretch;
    }

    property var loadingImage;
    property var component;

    function createLoadingImages()
    {
        component = Qt.createComponent("DynamicImage.qml");
        if (component.status === Component.Ready)
            finishCreation();
        else
            component.statusChanged.connect(finishCreation);
    }

    function finishCreation() {
         if (component.status === Component.Ready) {
             loadingImage = component.createObject(imageHighlight, {"source": "../assets/No-Art.png"});
             if (loadingImage === null) {
                 // Error Handling
                 console.log("Error creating image object");
             }
         }
         else if (component.status === Component.Error) {
             // Error Handling
             console.log("Error loading component:", component.errorString());
         }
    }

    DynamicImage {
        id: image;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            horizontalCenter: parent.horizontalCenter;
        }
        onProgressChanged: gridItem.imageProgress = image.progress;

        onStatusChanged: {

            if (status !== Image.Ready) {
                gridItem.imageLoaded = true;
                imageHighlight.createLoadingImages();
            }

            else {
                gridItem.imageLoaded = false;
                if (imageHighlight.loadingImage !== null && imageHighlight.loadingImage !== undefined)
                    imageHighlight.loadingImage.destroy();
            }
        }
    }
}

