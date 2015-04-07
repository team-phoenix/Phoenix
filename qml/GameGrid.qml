import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

Rectangle {
    id: gameGrid;
    color: "#262626";
    height: 500;
    width: 500;

    property string itemBackgroundColor: "red";
    property bool showSystem: true;
    property real zoomFactor: 1;
    property bool zoomSliderPressed: false;
    property bool resizeGrid: false;


    Rectangle {
        // bottomBorder;
        z: gridView.z + 1;
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: "#0b0b0b";
    }

    Component.onCompleted: {
        root.itemInView = "grid";
    }

    onZoomFactorChanged: {
        if (gridView.cellHeight * zoomFactor !== gridView.cellHeight)
            resizeGrid = true;
    }

    WelcomeView {
        z: parent.z + 1;
        opacity: gridView.count == 0 ? 1.0 : 0;
        Behavior on opacity {
            PropertyAnimation {duration: 200;}
        }

        anchors {
            fill: parent;
            margins: 24;
        }
    }

    Rectangle {
        id: scrollHandle;
        color: "red"
        radius: 3;

        property int minHeight: 20;

        // Compensate for the min height and the bottom margin
        y: 2 + parent.height * gridView.visibleArea.yPosition * ( ( parent.height - minHeight - scrollBackground.bottomMargin + 4 ) / parent.height );
        z: parent.z + 2;
        height: Math.max( parent.height * gridView.visibleArea.heightRatio, minHeight );
        gradient: Gradient {
            GradientStop {position: 0.0; color: "#2b2a2b";}
            GradientStop {position: 1.0; color: "#252525";}
        }

        anchors {
            right: parent.right;
            left: scrollBackground.left;
            margins: 2;
        }


        MouseArea {
            id: scrollHandleMouseArea
            anchors.fill: parent
            anchors.margins: -10
            hoverEnabled: true
        }

        Rectangle {
            color: "#525254";
            anchors {
                top: parent.top;
                left: parent.left;
                leftMargin: 1;
                rightMargin: 1;
                right: parent.right;
            }
            height: 1;
        }

        Rectangle {
            anchors {
                left: parent.left;
                top: parent.top;
                bottom: parent.bottom;
                bottomMargin: 1;
                topMargin: 1;
            }
            color: "#414142";
            width: 1;
        }

        Rectangle {
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
                bottomMargin: 1;
                topMargin: 1;
            }
            color: "#414142";
            width: 1;
        }

        Rectangle {
            color: "#323233";
            anchors {
                bottom: parent.bottom;
                left: parent.left;
                leftMargin: 1;
                rightMargin: 1;
                right: parent.right;
            }
            height: 1;
        }
    }

    Rectangle {
        property bool hovered: scrollBackgroundMouseArea.containsMouse | scrollHandleMouseArea.containsMouse;
        property int bottomMargin: 20;
        id: scrollBackground;
        radius: 3;
        color: "#1c1c1c";
        height: parent.height - 20;
        width: hovered ? 20 : 12;
        z: parent.z + 1;

        anchors {
            right: parent.right;
        }

        border {
            width: 1;
            color: "#1a1a1a";
        }

        Behavior on width {
            PropertyAnimation {
                easing {
                    type: Easing.Linear;
                }
                duration: 100;
            }
        }

        MouseArea {
            id: scrollBackgroundMouseArea
            anchors.fill: parent
            anchors.margins: -10
            hoverEnabled: true
        }
    }



    GridView {
        id: gridView;

        property bool indexUpdated: false;
        property string titleToDelete: "";
        property bool shrink: false;
        property int queuedIndex: 0;
        property var moveCurrentDirection: -1;
        property bool showRightClickMenu: false;

        boundsBehavior: Flickable.StopAtBounds;

        Behavior on cellWidth {
            PropertyAnimation {duration: 50;  easing.type: Easing.Linear;}
        }

        Behavior on cellHeight {
            PropertyAnimation {duration: 50;  easing.type: Easing.Linear;}
        }

        anchors {
            fill: parent;
            leftMargin: (parent.width >= cellWidth) ? ((parent.width % cellWidth) / 2) : 0;
            rightMargin: leftMargin;
            bottomMargin: 40;
        }

        cellHeight: 100 * gameGrid.zoomFactor;
        cellWidth: 100 * gameGrid.zoomFactor;
        model: phoenixLibrary.model();
        highlightFollowsCurrentItem: false;
        currentIndex: 0;

        highlight: GridHighlighter {
            //visible: root.gridFocus;
            height: gridView.currentItem.height;
            width: gridView.currentItem.width;
            y: gridView.currentItem.y;
            x: gridView.currentItem.x;
        }

        MouseArea {
            id: dismissalMouseArea;
            anchors.fill: parent;
            enabled: gridView.showRightClickMenu;
            onClicked: gridView.showRightClickMenu = false;
        }

        delegate: GridItem {
            id: gridItem;
            height: gridView.cellHeight;
            width: gridView.cellWidth;
            z: index == gridView.currentIndex ? 2 : 0;

        }
    }

}
