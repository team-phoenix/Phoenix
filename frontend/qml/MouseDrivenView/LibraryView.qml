import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Item {
    id: libraryView;

    Rectangle {
        anchors {
            top: parent.top; bottom: parent.bottom; left: parent.left;
            topMargin: headerArea.height; leftMargin: 30;
        }
        width: 420;
        color: "transparent";

        ListView {
            id: listView;
            anchors.fill: parent;
            spacing: 3;
            model: DefaultCoreModel { }

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
                        anchors { right: parent.right; rightMargin: coreExists.width; }

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
                        id: coreExists;
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
