import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Item {
    width: 100
    height: 6

    DropShadow {
        anchors.fill: source;
        source: selectionArea;
        transparentBorder: true;
        verticalOffset: 0;
        horizontalOffset: 1;
        color: "#b0000000";
        radius: 8.0;
        samples: radius * 2;
    }

    Rectangle  {
        id: selectionArea;
        anchors.fill: parent;

        ColumnLayout {
            anchors.fill: parent;
            spacing: 0;

            Rectangle {
                height: 100;
                Layout.fillWidth: true;
                color: "orange";

            }

            StackView {
                id: sectionsAreaStackView;
                Layout.fillHeight: true;
                Layout.fillWidth: true;

                initialItem: platformsView;

                Component {
                    id: platformsView;
                    PlatformsView {

                    }
                }

                Component {
                    id: favoritesView;

                    CollectionsView {

                    }
                }

                delegate: StackViewDelegate {
                        function transitionFinished(properties)
                        {
                            properties.exitItem.opacity = 1
                            properties.exitItem.y = 0;
                        }

                        pushTransition: StackViewTransition {
                            PropertyAnimation {
                                target: enterItem
                                property: "opacity"
                                from: 0
                                to: 1
                            }
                            PropertyAnimation {
                                target: exitItem
                                property: "opacity"
                                from: 1
                                to: 0
                            }
                            PropertyAnimation {
                                target: enterItem
                                property: "y"
                                from: enterItem.height;
                                to: 0;
                            }

                            PropertyAnimation {
                                target: exitItem
                                property: "y"
                                from: exitItem.height;
                                to: 0;
                            }
                        }
                    }
            }

            ListView {
                id: sectionAreaToolbar;
                height: 62;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                spacing: 0;

                interactive: false;

                orientation: ListView.Horizontal;

                model: ListModel {
                    ListElement { bgColor: "gray"; label: "Add"; }
                    ListElement { bgColor: "darkgray"; label: "Favorites"; }
                    ListElement { bgColor: "gray"; label: "Games"; }
                    ListElement { bgColor: "darkgray"; label: "Settings"; }

                }

                delegate: Rectangle {
                    height: parent.height;
                    width: sectionAreaToolbar.width / sectionAreaToolbar.count;
                    color: bgColor;

                    Text {
                        anchors.centerIn: parent;
                        text: label;

                        font {
                            pixelSize: PhxTheme.sectionArea.normalFontSize;
                        }
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            if ( index == 1 ) {
                                sectionsAreaStackView.push( { item: favoritesView, replace: true } );
                            }

                            else if ( index == 2 ) {
                                sectionsAreaStackView.push( { item: platformsView, replace: true } );
                            }
                        }
                    }

                }
            }
        }
    }
}

