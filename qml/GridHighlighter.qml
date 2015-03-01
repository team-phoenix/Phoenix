import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {

    id: highlighter;
    z: highlighterSource.z;

    property bool enableDropShadow: true;
    property var dropshadowObject: undefined;

    function createDropShadow()
    {
        if (dropshadowObject !== undefined && !enableDropShadow) {
            dropshadowObject.destroy();
            dropshadowObject = undefined;
            opacityMask.visible = true;
        }
        else if (enableDropShadow) {
            opacityMask.visible = false;
            if (dropshadowObject === undefined) {
                dropshadowObject = Qt.createQmlObject(
                    "
                        import QtQuick 2.3
                        import QtGraphicalEffects 1.0

                        DropShadow {
                            id: dropSource;
                            anchors.fill: opacityMask;
                            source: opacityMask;
                            color: '#dc133a';
                            horizontalOffset: 0
                            verticalOffset: 1
                            radius: 16
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
        id: maskSource;
        visible: false;
        color: "transparent";
        anchors {
            fill: highlighterSource;
        }
        radius: highlighterSource.radius
        border {
            width: 6;
            color: "black"
        }

        Rectangle {
            anchors {
                fill: parent;
                margins: parent.border.width - 4;
            }
            radius: parent.radius;
            color: "transparent";
            border {
                width: 2;
                color: "black"
            }
            Rectangle {
                anchors {
                    fill: parent;
                    margins: 1;
                }
                radius: parent.radius;
                color: "transparent";
                border {
                    width: 2;
                    color: "black"
                }
                Rectangle {
                    anchors {
                        fill: parent;
                        margins: 1;
                    }
                    radius: parent.radius;
                    color: "transparent";
                    border {
                        width: 2;
                        color: "black"
                    }
                    Rectangle {
                        anchors {
                            fill: parent;
                            margins: 1;
                        }
                        radius: parent.radius;
                        color: "transparent";
                        border {
                            width: 2;
                            color: "black"
                        }
                        Rectangle {
                            anchors {
                                fill: parent;
                                margins: 1;
                            }
                            radius: parent.radius;
                            color: "transparent";
                            border {
                                width: 2;
                                color: "black"
                            }
                            Rectangle {
                                anchors {
                                    fill: parent;
                                    margins: 1;
                                }
                                radius: parent.radius;
                                color: "transparent";
                                border {
                                    width: 2;
                                    color: "black"
                                }
                                Rectangle {
                                    anchors {
                                        fill: parent;
                                        margins: 1;
                                    }
                                    radius: parent.radius;
                                    color: "transparent";
                                    border {
                                        width: 2;
                                        color: "black"
                                    }
                                    Rectangle {
                                        anchors {
                                            fill: parent;
                                            margins: 1;
                                        }
                                        radius: parent.radius;
                                        color: "transparent";
                                        border {
                                            width: 2;
                                            color: "black"
                                        }
                                        Rectangle {
                                            anchors {
                                                fill: parent;
                                                margins: 1;
                                            }
                                            radius: parent.radius;
                                            color: "transparent";
                                            border {
                                                width: 2;
                                                color: "black"
                                            }
                                            Rectangle {
                                                anchors {
                                                    fill: parent;
                                                    margins: 1;
                                                }
                                                radius: parent.radius;
                                                color: "transparent";
                                                border {
                                                    width: 2;
                                                    color: "black"
                                                }
                                                Rectangle {
                                                    anchors {
                                                        fill: parent;
                                                        margins: 1;
                                                    }
                                                    radius: parent.radius;
                                                    color: "transparent";
                                                    border {
                                                        width: 2;
                                                        color: "black"
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: highlighterSource;
        z: 100;
        visible: false;
        width: gridView.currentItem.artworkImage.width + 12;
        height: gridView.currentItem.artworkImage.height + 12;
        anchors {
            centerIn: highlighter;
        }

        radius: 12;
        color: "#b0000000"

        Component.onCompleted: state = "grow";

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
                    width: gridView.currentItem.artworkImage.width + 12;
                    height: gridView.currentItem.artworkImage.height + 12;

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

        Rectangle {
            anchors {
                fill: parent;
                margins: 1;
            }


            radius: parent.radius - 1;

            gradient: Gradient {
                GradientStop {color: "#f06413"; position: 0.0}
                GradientStop {color: "#dc133a"; position: 1.0}
            }
            smooth: true;

            Rectangle {
                color: "transparent";
                anchors {
                    fill: parent;
                }
                border {
                    color: "white";
                    width: 1;
                }
                opacity: 0.3;
                radius: parent.radius;
                smooth: true;
            }

            Rectangle {
                id: outerInnerBorder;
                anchors {
                    fill: parent;
                    margins: 9;
                }

                gradient: Gradient {
                    GradientStop {color: "#f38a4d"; position: 0.0}
                    GradientStop {color: "#e44d6a"; position: 1.0}
                }


                radius: parent.radius + 1;


                Rectangle {
                    id: innerBackground;
                    radius: parent.radius;
                    anchors {
                        fill: parent;
                        margins: 1;
                    }

                    gradient: Gradient {
                        GradientStop {color: "#b44b0e"; position: 0.0}
                        GradientStop {color: "#a50e2b"; position: 1.0}
                    }

                    Rectangle {
                        anchors {
                            fill: parent;
                            margins: 2;
                        }
                        color: "black";
                        radius: parent.radius;
                    }
                }
            }
        }
    }

    OpacityMask {
        id: opacityMask;
        visible: !highlighter.enableDropShadow;
        anchors {
            fill: highlighterSource;
            margins: -12;
        }
        source: highlighterSource;
        maskSource: maskSource;
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
