import QtQuick 2.4
import QtGraphicalEffects 1.0

Item  {
    id: imageHighlight;


    property bool imageVisible: image.visible && image.status === Image.Ready;
    property alias artwork: image;

    RectangularGlow {
        visible: index !== gridView.currentIndex && parent.imageVisible && !gridItem.itemDeleted;
        anchors.fill: blackBorder;
        glowRadius: 7;
        spread: 0.1;
        color: "#b0000000";
        cornerRadius: 2;
    }


    Rectangle {
        id: blackBorder;
        radius: 3;
        anchors {
            fill: borderImage;
            margins: -1;
        }
        visible: parent.imageVisible && !gridItem.itemDeleted;
        onHeightChanged: gridItem.paintedHeight = height;
        onWidthChanged: gridItem.paintedWidth = width;
        z: borderImage.z - 1;
        color: "black";
    }

    BorderImage {
        z: image.z + 1;
        id: borderImage;
        visible: parent.imageVisible && !gridItem.itemDeleted;
        source: "../assets/glow-mask.png"
        width: height * image.aspectRatio;
        height: image.paintedHeight;
        anchors {
            centerIn: image;
            //horizontalCenter: image.horizontalCenter;
        }


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
             loadingImage = component.createObject(image, {"source": "../assets/No-Art.png"});
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


        //verticalAlignment: Image.AlignBottom;
        //horizontalAlignment: Image.AlignHCenter;

        onProgressChanged: gridItem.imageProgress = image.progress;

        /*onStatusChanged: {

            if (status !== Image.Ready) {
                gridItem.imageLoaded = true;
                imageHighlight.createLoadingImages();
            }

            else {
                gridItem.imageLoaded = false;
                if (imageHighlight.loadingImage !== null && imageHighlight.loadingImage !== undefined)
                    imageHighlight.loadingImage.destroy();
            }
        }*/
    }

    Column {
        id: textColumn;
        anchors {
            top: image.bottom;
            topMargin: 5;
            horizontalCenter: image.horizontalCenter;
        }

        spacing: 0;
        //x: image.width - image.paintedWidth

        width: parent.width;

        MarqueeText {
            id: titleLabel;
            text: gridItem.titleName;
            anchors.horizontalCenter: parent.horizontalCenter;
            width: parent.width;
            textColor: "#f1f1f1";
            interval: 17;
            fontSize: 10;
            running: index == gridView.currentIndex || image.hovered ? true : false;
        }

        Text {
            id: systemLabel;
            visible: gameGrid.showSystem;
            renderType: Text.QtRendering;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            text: gridItem.systemName;
            color: "#f1f1f1";
            font {
                pixelSize: 10;
                family: "Sans";
            }

            elide: Text.ElideRight;
            horizontalAlignment: Text.AlignHCenter;
        }
    }
}

