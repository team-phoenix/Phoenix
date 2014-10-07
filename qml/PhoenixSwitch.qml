import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1


Switch {
    id: vsyncSwitch;
    anchors.right: parent.right;
    style: SwitchStyle {
        groove: Rectangle {
            color: "#1c1c1c";
            implicitWidth: 50;
            implicitHeight: 23;

            border {
                width: 1;
                color: control.checked ? "#3fe86b" : "#e8433f";
            }
            radius: 3;

            RowLayout {
                anchors.fill: parent;
                spacing: 8;
                Image {
                    source: "../assets/dot.png";
                    sourceSize {
                        height: 12;
                        width: 12;
                    }
                    height: 12;
                    width: 12;

                    anchors {
                        left: parent.left;
                        leftMargin: 8;
                        verticalCenter: parent.verticalCenter;
                    }
                }
                Image {
                    source: "../assets/x.png";
                    sourceSize {
                        height: 12;
                        width: 12;
                    }
                    height: 12;
                    width: 12;

                    anchors {
                        right: parent.right;
                        rightMargin: 8;
                        verticalCenter: parent.verticalCenter;
                    }
                }
            }
        }
        handle: Item {
            //implicitHeight: 16;
            width: 24;
            Rectangle {
                id: mainHandle;
                radius: 3;
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#424242";}
                    GradientStop {position: 1.0; color: "#313130";}
                }
                anchors {
                    fill: parent;
                    margins: 2;
                }

                border {
                    width: 1;
                    color: "#0b0b0b";
                }


                Rectangle {
                    visible: false;

                    anchors {
                        top: parent.top;
                        topMargin: mainHandle.border.width;
                        left: parent.left;
                        leftMargin: mainHandle.border.width + 2;
                        right: parent.right;
                        rightMargin: mainHandle.border.width + 2;
                    }
                    radius: 3;
                    smooth: true;
                    height: 1;
                    color: "#595959";
                }

                Rectangle {
                    visible: false;
                    anchors {
                        left: parent.left;
                        leftMargin: mainHandle.border.width;
                        top: parent.top;
                        topMargin: mainHandle.border.width + 2;
                        bottom: parent.bottom;
                        bottomMargin: mainHandle.border.width + 2;
                    }
                    width: 1;
                    color: "#4d4d4d";
                }
            }
        }
    }
}
