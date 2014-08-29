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
