import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0

ScrollView {
    width: 100
    height: 62;

    style: ScrollViewStyle {

        scrollBarBackground: Item {
            width: 12;

            Rectangle {
                id: leftBorder;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }

                width: 1;
                color: "white";
                opacity: 0.1;
            }

            Rectangle {
                color: "black";
                anchors.fill: parent;
                opacity: 0.2;
            }
        }

        incrementControl: Item {

        }

        decrementControl: Item {
        }

        handle: Item {
            implicitWidth: 5;
            implicitHeight: 30;
            x: 4;

            DropShadow {
                anchors.fill: source;
                source: handleRectangle;
                verticalOffset: 0;
                horizontalOffset: 0;
                radius: 8;
                samples: radius * 2;
                color: "red";
                transparentBorder: true;
            }

            Rectangle {
                id: handleRectangle;
                color: "white";
                radius: 4;
                anchors.fill: parent;
            }
        }

    }

}

