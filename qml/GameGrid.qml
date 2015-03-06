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



    PhoenixScrollView {
        id: scrollView;
        focus: root.gridFocus;
        onFocusChanged: {
            //console.log(focus ? "grid has focus" : "grid lost focus");
            //if (focus)
                //scrollView.forceActiveFocus();
        }

        anchors {
            fill: parent;
        }

        Keys.forwardTo: headerBar.textField;
        Keys.priority: Keys.BeforeItem;
        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_Right:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexRight();
                    event.accepted = true;
                    break;
                case Qt.Key_Left:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexLeft();
                    event.accepted = true;
                    break;
                case Qt.Key_Up:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection =  gridView.moveCurrentIndexUp();
                    event.accepted = true;
                    break;
                case Qt.Key_Down:
                    gridView.shrink = true;
                    gridView.moveCurrentDirection = gridView.moveCurrentIndexDown();
                    event.accepted = true;
                    break;

                default:
                    //headerBar.textField.carriedEvent = String.fromCharCode(event.key);
                    //headerBar.textField.focus = true;

                    break;
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


            Text {
                visible: gridView.count == 0;
                anchors.centerIn: parent;
                text: "0 Games Were Found, (PLACE HOLDER)";
                color: "#f1f1f1";
                font {
                    family: "Sans";
                    pixelSize: 32;
                }
            }

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

            delegate: GridItem {
                id: gridItem;
                height: gridView.cellHeight;
                width: gridView.cellWidth;
                z: index == gridView.currentIndex ? 2 : 0;


            }
        }
    }
}
