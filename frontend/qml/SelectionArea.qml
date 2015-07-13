import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0

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

            ScrollView {
                Layout.fillHeight: true;
                Layout.fillWidth: true;

                ListView {
                    id: listView;
                    spacing: 0;
                    model: PlatformsModel {
                        id: platformsModel;

                    }
                        /*ListModel {
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Super Nintendo"; }
                        ListElement { platform: "Sega Genesis"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }
                        ListElement { platform: "Nintendo Entertainment System"; }

                    }*/

                    delegate: Rectangle {
                        color: "yellow";
                        height: 25;
                        Layout.fillWidth: true;

                        Text {
                            text: listView.model.get( index );
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                leftMargin:  12;
                            }

                            font {
                                pixelSize: 12;
                            }

                            color: root.normalFontColor;

                            MouseArea {
                                anchors.fill: parent;
                                onClicked: {
                                    //platformsModel.move( index, index + 1 );
                                }
                            }
                        }


                    }
                }
            }

            Rectangle {
                color: "purple";
                height: 62;
                Layout.fillWidth: true;
            }
        }
    }
}

