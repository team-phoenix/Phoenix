import QtQuick 2.4
import QtQuick.Controls 1.2;
import QtQuick.Controls.Styles 1.2;

Image {
    id: image;

    property bool locallyCache: true;
    property real aspectRatio: paintedWidth / paintedHeight;

    width: gridItem.itemDeleted ? 0 : parent.width;
    source: !artwork ? "qrc:/assets/No-Art.png" : artwork;
    onStatusChanged: {
        if (status === Image.Error)
            source = "qrc:/assets/No-Art.png";
    }


    fillMode: Image.PreserveAspectFit;
    asynchronous: true;
    property bool hovered: false;

    sourceSize {
        height: 500;
        width: 500;
    }

    anchors {
        top: parent.top;
        bottom: parent.bottom;
        horizontalCenter: parent.horizontalCenter;
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
            if (mouse.button == Qt.RightButton) {
                if (gridView.showRightClickMenu)
                    gridView.showRightClickMenu = false;
                else
                    gridView.showRightClickMenu = true;
            }
            else {
                gridView.shrink = true;
            }
            gridView.queuedIndex = index;
        }
    }
}
