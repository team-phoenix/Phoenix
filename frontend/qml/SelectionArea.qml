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

                Button {
                    anchors.centerIn: parent;
                    text: qsTr( "All" );
                    onClicked: contentArea.contentLibraryModel.setFilter(  LibraryModel.SystemRole, "LIKE ?", "%%" );

                }
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

                    header: Rectangle {
                        color: "lightblue";
                        height: 36;

                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        Text {
                            text: qsTr( "SYSTEMS" );
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                leftMargin: 12;
                            }

                            font {
                                pixelSize: PhxTheme.sectionArea.headerFontSize;
                                bold: true;
                            }

                            color: PhxTheme.common.baseFontColor;
                        }
                    }

                    delegate: Rectangle {
                        color: index % 2 == 0 ? "yellow" : "lightgreen";
                        height: 24;

                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        Text {
                            id: platformText;
                            text: listView.model.get( index );
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                leftMargin:  24;
                            }

                            font {
                                pixelSize: PhxTheme.sectionArea.normalFontSize;
                            }

                            color: PhxTheme.common.baseFontColor;

                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                contentArea.contentLibraryModel.setFilter( LibraryModel.SystemRole, "= ?", platformText.text );
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

