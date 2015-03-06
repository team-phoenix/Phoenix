import QtQuick 2.4
import QtQuick.Controls 1.2;
import QtQuick.Controls.Styles 1.2;
import QtGraphicalEffects 1.0

Image {
    id: image;

    property bool locallyCache: true;
    property real aspectRatio: -1;
    property bool imageLoaded: false;
    property real imageProgress: 1.0;
    property bool itemDeleted: false;
    property bool hovered: false;
    property bool completelyLoaded: false;

    source: !artwork ? "qrc:/assets/No-Art.png" : artwork;
    fillMode: Image.PreserveAspectFit;
    asynchronous: true;

    sourceSize {
        height: 500;
        width: 500;
    }

    anchors {
        top: parent.top;
        bottom: parent.bottom;
        horizontalCenter: parent.horizontalCenter;
    }

    function properlyResizeImage(old_width)
    {
        // The point of this is to completely remove half-pixels from the images.
        // Some artwork comes a height such as, 145.75; the same goes for the width.
        // The code below rounds off these values and stretches the image to fit the
        // integer height and width.
        // The image's fidelity is still preserved, since the stretch is extremely minimal.

        var new_w = old_width;

        if (!(paintedWidth % 1 === 0.0)) {
            new_w = Math.round(old_width);
        }

        if (new_w % 2 == 0)
            new_w += 1;

        // This makes the height and width always odd, and so the highlighter will always
        // be centered perfectly. It's wonderful.

        var new_h = new_w * image.aspectRatio;

        if (!(new_h % 1 === 0.0)) {
            new_h = Math.round(new_h);
        }

        if (new_h % 2 == 0)
            new_h += 1;

        image.fillMode = Image.Stretch;
        image.height = new_h;
        image.width = new_w;

    }

    onStatusChanged: {
        if (status === Image.Error) {
            source = "qrc:/assets/No-Art.png";
            image.aspectRatio = -1.0;
            completelyLoaded = false;
        }

        else if (status === Image.Ready) {
            if (image.aspectRatio === -1)
                image.aspectRatio = image.paintedHeight / image.paintedWidth;
            properlyResizeImage(image.paintedHeight);
            completelyLoaded = true;
        }

    }

    onWidthChanged: {
        if (width === 0) {
            if (gridView.titleToDelete !== "")
                phoenixLibrary.deleteRow(gridView.titleToDelete);
        }
    }

    Rectangle {
        id: topBorder;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "white";
        opacity: 0.35;
    }

    Rectangle {
        id: bottomBorder;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "white";
        opacity: 0.15;
    }

    Rectangle {
        id: leftBorder;
        anchors {
            bottom: bottomBorder.top;
            top: topBorder.bottom;
            left: parent.left;
        }

        width: 1;
        color: "white";
        opacity: 0.20;
    }

    Rectangle {
        id: rightBorder;
        anchors {
            bottom: bottomBorder.top;
            top: topBorder.bottom;
            right: parent.right;
        }

        width: 1;
        color: "white";
        opacity: 0.20;
    }

    RectangularGlow {
        id: dropSource;
        anchors.fill: customBorder;
        glowRadius: 5;
        spread: 0.3;
        color: "black";
        z: customBorder.z;
        cornerRadius: customBorder.radius + glowRadius;
    }


    CustomBorder {
        id: customBorder;
        color: "black";
        radius: 0;
    }

    MouseArea {
        id: mouseArea;
        propagateComposedEvents: true;
        anchors.fill: parent;
        hoverEnabled: true;
        enabled: true;
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onEntered: parent.hovered = true;
        onExited: parent.hovered = false;

        onDoubleClicked: {
            root.playGame(title ,system, filename, phoenixLibrary.getSystem(system));
        }

        onClicked: {
            gridView.queuedIndex = index;
            if (root.keyBoardFocus !== 2)
                root.keyBoardFocus = 2;

            if (mouse.button == Qt.RightButton) {
                if (gridView.showRightClickMenu)
                    gridView.showRightClickMenu = false;
                else
                    gridView.showRightClickMenu = true;
            }
            else {
                if (index !== gridView.currentIndex)
                    gridView.shrink = true;
            }
        }
    }
}
