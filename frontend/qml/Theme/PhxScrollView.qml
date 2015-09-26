import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0

ScrollView {
    style: ScrollViewStyle {

        scrollBarBackground: Item {
            width: 12;
        }

        incrementControl: Item { }

        decrementControl: Item { }

        handle: Item {
            implicitWidth: 8;
            implicitHeight: 30;
            x: 0;

            DropShadow {
                anchors.fill: source;
                source: handleRectangle;
                verticalOffset: 0;
                horizontalOffset: 0;
                radius: 8;
                samples: radius * 2;
                color: "black";
                transparentBorder: true;
            }

            Rectangle {
                id: handleRectangle;
                color: "#75FFFFFF";
                anchors.fill: parent;
            }
        }
    }
}
