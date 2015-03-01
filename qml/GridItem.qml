import QtQuick 2.4
import QtGraphicalEffects 1.0

Item  {
    id: gridItem;
    property bool imageVisible: image.visible && image.status === Image.Ready;
    property alias artworkImage: image;

    onWidthChanged: {
        if (image.completelyLoaded) {
            image.properlyResizeImage(width * 0.5);
        }
    }

    GridImage {
        id: image;

        anchors.centerIn: parent;
        height: parent.height * 0.5;
        width: parent.width * 0.5;

    }

    Column {
        id: textColumn;

        anchors {
            top: image.bottom;
            topMargin: (index == gridView.currentIndex) ? 20 : 5;
            horizontalCenter: image.horizontalCenter;
        }

        Behavior on anchors.topMargin {
            PropertyAnimation {
                duration: 150;
            }
        }

        spacing: 0;
        width: parent.width;

        MarqueeText {
            id: titleLabel;
            text: title;
            anchors.horizontalCenter: parent.horizontalCenter;
            width: parent.width;
            textColor: "#f1f1f1";
            animationInterval: 100;
            fontSize: 10;
            mousedOrSelected: index == gridView.currentIndex || image.hovered ? true : false;
        }

        Text {
            id: systemLabel;
            visible: gameGrid.showSystem;
            renderType: Text.QtRendering;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            text: system;
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

