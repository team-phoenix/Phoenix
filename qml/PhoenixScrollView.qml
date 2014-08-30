import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

ScrollView {
    style: ScrollViewStyle {
        id: scrollStyle;
        property int handleWidth: 10
        frame: Rectangle {
            color: gameGrid.color;
            //width: 0;
            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#1a1a1a";
            }
        }

        transientScrollBars: true;
        scrollToClickedPosition: true;
        handleOverlap: -3;


        handle: Rectangle {
            color: "red"
            radius: 3;
            x: 3;
            implicitWidth: scrollStyle.handleWidth - (x * 2);
            implicitHeight: 30;
            gradient: Gradient {
                GradientStop {position: 0.0; color: "#2b2a2b";}
                GradientStop {position: 1.0; color: "#252525";}
            }

            Rectangle {
                color: "#525254";
                anchors {
                    top: parent.top;
                    left: parent.left;
                    leftMargin: 1;
                    rightMargin: 1;
                    right: parent.right;
                }
                height: 1;
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    topMargin: 1;
                }
                color: "#414142";
                width: 1;
            }

            Rectangle {
                anchors {
                    right: parent.right;
                    top: parent.top;
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    topMargin: 1;
                }
                color: "#414142";
                width: 1;
            }

            Rectangle {
                color: "#323233";
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    leftMargin: 1;
                    rightMargin: 1;
                    right: parent.right;
                }
                height: 1;
            }
        }

        scrollBarBackground: Rectangle {
            radius: 3;
            color: "#1c1c1c";
            height: control.height;
            width: styleData.hovered ? 16 : 12;
            onWidthChanged:  {
                if (styleData.hovered)
                    scrollStyle.handleWidth = 16;
                else
                    scrollStyle.handleWidth = 12;
            }

            border {
                width: 1;
                color: "#1a1a1a";
            }
        }
    }
}
