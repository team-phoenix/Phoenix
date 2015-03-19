import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {

    id: highlighter;
    z: highlighterSource.z;

    property bool enableDropShadow: false;
    property var dropshadowObject: undefined;

    function createDropShadow()
    {
        if (dropshadowObject !== undefined && !enableDropShadow) {
            dropshadowObject.destroy();
            dropshadowObject = undefined;
            opacityMask.visible = true;
        }
        else if (enableDropShadow) {
            highlighterSource.visible = false;
            if (dropshadowObject === undefined) {
                dropshadowObject = Qt.createQmlObject(
                    "
                        import QtQuick 2.3
                        import QtGraphicalEffects 1.0

                        DropShadow {
                            id: dropSource;
                            anchors.fill: highlighterSource;
                            source: highlighterSource;
                            color: 'black';
                            visible: root.keyBoardFocus === 2 && parent.enableDropShadow;
                            horizontalOffset: 0
                            verticalOffset: 1
                            radius: 8;
                            samples: radius * 2;
                            transparentBorder: true;
                        }", highlighter, "GridItemDropShadow");
            }
        }
    }

    onEnableDropShadowChanged: createDropShadow();
    Component.onCompleted: createDropShadow();

    // This mask is a complete hack job, but hey it works!
    // The more you increase the border.width on the Rectangle, the inner rectangle's
    // radius is not perfectly preserved, and eventually will approach 0.
    // This causes the Rectangle to have a curved outer perimeter, but a square inner perimeter,
    // which isn't what we want. This hack job fixes that, cause overlapping the borders of multiple
    // Rectangles.

    // The highlighter is only constructed once, and so the performance will be fine.

    Rectangle {
        id: highlighterSource;
        z: 1;
        visible: root.keyBoardFocus === 2 && !parent.enableDropShadow;
        width: gridView.currentItem.artworkImage.paintedWidth + 9;
        height: gridView.currentItem.artworkImage.paintedHeight + 9;
        anchors {
            bottom: parent.bottom;
            bottomMargin: -6;
            horizontalCenter: parent.horizontalCenter;
        }

        radius: 5;
        color: "#f7b188";

        Component.onCompleted: state = "grow";

        Rectangle {
            id: highlighterBorder;
            anchors {
                fill: parent;
                topMargin: 1;
            }
            z: parent.z + 1;
            color:  "#ffcc4d";
            radius: parent.radius - 1;
            gradient: Gradient {
                GradientStop {color: "#f06413"; position: 0.0}
                GradientStop {color: "#dc133a"; position: 1.0}
            }
        }

        Rectangle {
            anchors {
                centerIn: parent;
                verticalCenterOffset: 4;
            }
            z: parent.z + 1;
            height: gridView.currentItem.artworkImage.paintedHeight;
            width: gridView.currentItem.artworkImage.paintedWidth + 4;

            radius: parent.radius * 0.4;
            color: "white";
            gradient: Gradient {
                GradientStop {color: "black"; position: 0.0}
                GradientStop {color: "black"; position: 0.99}
                GradientStop {color: "white"; position: 1.0}
            }
            opacity: 0.4;
        }

        states: [
           State {
                name: "shrink";
                when: gridView.shrink;
                PropertyChanges {
                    id: shrinkAnimation;
                    target: highlighterSource;
                    height: 0;
                    width: 0;
                    y: (gridView.currentItem.height + gridView.currentItem.y) / 2;
                    x: (gridView.currentItem.x + gridView.currentItem.width) / 2;
                }
            },
            State {
                id: grow;
                name: "grow";
                PropertyChanges {
                    id: growAnimation;
                    target: highlighterSource;
                    width: gridView.currentItem.artworkImage.paintedWidth + 19;
                    height: gridView.currentItem.artworkImage.paintedHeight + 19;

                }
            }

        ]

        transitions: [
            Transition {
                from: "grow";
                to: "shrink";

                NumberAnimation {
                    properties: "width,height,x,y,opacity";
                    easing.type: Easing.InBack;
                    duration: 100;
                }

                onRunningChanged: {
                    if (!running) {

                        if (gridView.moveCurrentDirection !== -1) {
                            gridView.moveCurrentDirection;
                        }
                        else {
                            gridView.currentIndex = gridView.queuedIndex;
                        }

                        highlighterSource.state = "grow";
                    }
                }
            },
            Transition {
                from: "shrink";
                to: "grow";
                NumberAnimation {
                    properties: "width,height,x,y,opacity";
                    easing.type: Easing.OutBack;
                    duration: 150;
                }

                onRunningChanged: {
                    if (!running) {
                        gridView.shrink = false;
                        gridView.moveCurrentDirection = -1;
                    }
                }
            }
        ]
    }

    Desaturate {
        anchors.fill: source;
        source: highlighterSource;
        desaturation: 1.0;
        visible: root.keyBoardFocus !== 2;
    }

    RectangularGlow {
        visible: dropdownMenu.visible;
        anchors.fill: dropdownMenu;
        glowRadius: 3;
        spread: 0.3;
        color: "#f0000000";
        cornerRadius: dropdownMenu.radius + glowRadius;
    }

    Rectangle {
        color: "#4e4e4e";
        visible: gridView.showRightClickMenu;
        id: dropdownMenu;
        width: 100;
        height: 200;
        x: highlighterSource.x + highlighterSource.width + 12;
        anchors {
            //left: parent.right;
            //leftMargin: -10 * gameGrid.zoomFactor;
            verticalCenter: parent.verticalCenter;
            verticalCenterOffset: 65 / gameGrid.zoomFactor;
        }
        radius: 2;
        z: 2;


        RightClickMenu {
            id: rightClickMenu;
            anchors {
                fill: parent;
                margins: 1;
            }

            color: "#141414";
            radius: parent.radius;
        }
    }
}
