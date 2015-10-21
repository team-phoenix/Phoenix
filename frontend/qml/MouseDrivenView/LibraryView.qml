import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import vg.phoenix.models 1.0

Item {
    id: libraryView;

    Rectangle {

        anchors.centerIn: parent;
        height: 500;
        width: 300;
        color: "yellow";

        ListView {
            id: listView;
            anchors.fill: parent;

            model: DefaultCoreModel {

            }

            spacing: 3;

            delegate: Item {

                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                height: 25;

                RowLayout {
                    anchors.fill: parent;
                    spacing: 2;

                    Text {
                        text: system + ":";
                        height: parent.height;
                        Layout.fillWidth: true;
                    }

                    ComboBox {
                        id: comboBox;
                        model: cores;
                        height: parent.height;
                        width: 100;
                        anchors {
                            right: parent.right;
                        }

                        style: ComboBoxStyle {
                            background: Rectangle {
                                implicitHeight: control.height;
                                implicitWidth: control.width;
                                color: listView.model.coreExists( control.currentText ) ? "lightblue" : "red";
                            }
                        }

                        Component.onCompleted: {
                            // Work around because onCurrentTextChanged is fired when the ComboBox
                            // is constructed.
                            currentTextChanged.connect( function() {
                                listView.model.save( system, currentText );
                            });
                        }
                    }

                }


            }
        }

    }

}

