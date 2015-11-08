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
        width: 500;
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

                    RowLayout {
                        anchors.fill: parent;
                        spacing: 4;

                        Text {
                            text: system + ":";
                            height: parent.height;
                            Layout.fillWidth: true;
                            color: PhxTheme.common.highlighterFontColor;
                        }

                        ComboBox {
                            id: comboBox;
                            model: cores;
                            height: parent.height - 5;
                            width: 200;
                            anchors { right: parent.right; rightMargin: coreStatusIndicator.width; }

                            style: ComboBoxStyle {
                                background: Rectangle {
                                    implicitHeight: control.height;
                                    implicitWidth: control.width;
                                    color: "#FFFFFF";
                                }
                            }

                            Component.onCompleted: {

                                // Set the default core's index now
                                currentIndex = defaultCoreIndex;

                                // Work around because onCurrentTextChanged is fired when the ComboBox
                                // is constructed.
                                currentTextChanged.connect( function() {
                                    listView.model.save( system, currentText );
                                });
                            }
                        }

                        Rectangle {
                            id: coreStatusIndicator;
                            anchors { right: parent.right; }
                            width: 8;
                            height: comboBox.height;
                            color: listView.model.coreExists( comboBox.currentText ) ? PhxTheme.common.successColor : PhxTheme.common.errorColor;
                        }
                    }
                }
            }
        }
    }
}
