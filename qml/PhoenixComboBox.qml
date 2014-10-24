import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

ComboBox {
    anchors.right: parent.right;
    implicitWidth: 100;
    style: ComboBoxStyle {
        renderType: Text.QtRendering;
        background: Rectangle {
            id: backgroundRect;
            implicitHeight: 20;
            implicitWidth: 100;
            radius: 3;
            border {
                width: 1;
                color: "#e8433f";
            }

            gradient: Gradient {
                GradientStop {position: 0.0; color: "#2c2c2c";}
                GradientStop {position: 1.0; color: "#212121";}
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    topMargin: backgroundRect.border.width;
                    left: parent.left;
                    leftMargin: backgroundRect.border.width + 1;
                    right: parent.right;
                    rightMargin: backgroundRect.border.width + 1;
                }
                height: 1;
                color: "#454547";
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: backgroundRect.border.width;
                    left: parent.left;
                    leftMargin: backgroundRect.border.width + 1;
                    right: parent.right;
                    rightMargin: backgroundRect.border.width + 1;
                }
                height: 1;
                color: "#252525";
            }

            Rectangle {
                color: "#373738";
                width: 1;
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: backgroundRect.border.width + 2;
                    left: parent.left;
                    leftMargin: backgroundRect.border.width + 1;
                    top: parent.top;
                    topMargin: backgroundRect.border.width + 2;
                }
            }

            Rectangle {
                color: "#373738";
                width: 1;
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: backgroundRect.border.width + 2;
                    right: parent.right;
                    rightMargin: backgroundRect.border.width + 1;
                    top: parent.top;
                    topMargin: backgroundRect.border.width + 2;
                }
            }

        }

        label:Item {
            implicitHeight: 20;
            implicitWidth: 100;
            Text {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    verticalCenter: parent.verticalCenter;
                }

                renderType: Text.QtRendering;
                elide: Text.ElideRight;
                color: "#f1f1f1";
                font {
                    family: "Sans";
                    pixelSize: 10;
                }
                text: control.currentText;
            }
        }
    }
}
/*
                            PhoenixNormalButton {
                                id: playersBox;
                                width: 125;
                                property int num: devicesBox.count;
                                property int delegateHeight: 25;

                                property bool drop: false;
                                property var model:  {
                                    var mod = [];
                                    for (var i=0; i < num; ++i) {
                                        mod.push("Player " + (i + 1));
                                    }
                                    return mod;
                                }
                                property int currentIndex: 0;
                                onPressedChanged: {
                                    if (pressed) {
                                        if (drop)
                                            drop = false;
                                        else
                                            drop = true;
                                    }

                                }

                                text: model[currentIndex];


                                ListView {
                                    id: repeater;
                                    z: parent.z + 1;
                                    visible: parent.drop;
                                    anchors {
                                        top: parent.bottom;
                                    }
                                    model: parent.model;
                                    onCurrentIndexChanged: {
                                        inputMapper.deviceIndex = currentIndex;
                                        inputMapper.mapping = inputmanager.mappingForPort(currentIndex);
                                        inputMapper.device = inputmanager.getDevice(currentIndex);
                                    }

                                    delegate: Rectangle {
                                        color: "black";
                                        z: parent.z;
                                        width: playersBox.width + 12;
                                        height: playersBox.height;
                                        Text {
                                            anchors {
                                                left: parent.left;
                                                leftMargin: 12;
                                                verticalCenter: parent.verticalCenter;
                                            }
                                            font {
                                                family: "Sans";
                                                pixelSize: 11;
                                            }

                                            color: "#f1f1f1";

                                            text: modelData;
                                        }
                                    }
                                }
                            }

*/
