import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Item {
    id: librarySettingsView;

    Rectangle {
        anchors {
            top: parent.top; bottom: parent.bottom; left: parent.left;
            topMargin: headerArea.height; leftMargin: 30;
        }
        width: 600;
        color: "transparent";
        PhxScrollView {
            anchors.fill: parent;

            // The default of 20 just isn't fast enough
            __wheelAreaScrollSpeed: 100;

            ListView {
                id: listView;
                anchors.fill: parent;
                anchors.rightMargin: 10;
                spacing: 3;
                model: CoreModel { }

                delegate: Item {
                    anchors { left: parent.left; right: parent.right; }
                    height: 35;

                    Text {
                        anchors.left: parent.left;
                        anchors.verticalCenter: parent.verticalCenter;
                        Layout.fillWidth: true;
                        height: parent.height;
                        text: system + ":";
                        color: PhxTheme.common.highlighterFontColor;
                        verticalAlignment: Text.AlignVCenter;
                    }

                    Row {
                        anchors.right: parent.right;
                        anchors.rightMargin: 8;
                        height: parent.height;
                        spacing: 8;

                        Item {
                            anchors.verticalCenter: parent.verticalCenter;
                            width: 308;
                            height: parent.height;

                            ComboBox {
                                id: comboBox;
                                model: cores;
                                anchors.verticalCenter: parent.verticalCenter;
                                width: 300;
                                height: parent.height;

                                style: ComboBoxStyle {
                                    background: Rectangle {
                                        implicitHeight: control.height;
                                        implicitWidth: control.width;
                                        color: "#FFFFFF";
                                    }
                                }

                                Component.onCompleted: {

                                    // Set the current core's index now
                                    currentIndex = currentCoreIndex;

                                    // Workaround, onCurrentIndexChanged is fired when the ComboBox is constructed.
                                    currentIndexChanged.connect( function() {
                                        listView.model.save( system, currentIndex );
                                    });
                                }
                            }

                            Rectangle {
                                id: coreStatusIndicator;
                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: comboBox.right;
                                width: 8;
                                height: parent.height;
                                color: listView.model.coreExists( comboBox.currentText.replace( " (default)", "" ) ) ?
                                           PhxTheme.common.successColor : PhxTheme.common.errorColor;
                            }
                        }


                        Image {
                            id: resetCoreButton;
                            anchors.verticalCenter: parent.verticalCenter;
                            width: 15;
                            height: 15;
                            visible: comboBox.currentIndex !== defaultCoreIndex;
                            enabled: visible;
                            sourceSize { width: width; height: height; }
                            source: "close.svg";
                            fillMode: Image.PreserveAspectFit;
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: comboBox.currentIndex = defaultCoreIndex;
                            }
                        }
                        Item {
                            id: filler;
                            width: 15;
                            height: 15;
                            visible: !resetCoreButton.visible;
                            enabled: visible;
                        }
                    }
                }
            }
        }
    }
}
