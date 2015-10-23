import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

ScrollView {
    style: ScrollViewStyle {

        scrollBarBackground: Item { width: 0; }
        incrementControl: Item { }
        decrementControl: Item { }

        handle: Item {
            implicitWidth: 6;
            implicitHeight: 30;
            y: 0;
            x: -implicitWidth + 1;

            Rectangle {
                id: handleRectangle;
                color: "#20FFFFFF";
                anchors.fill: parent;
                // radius: parent.implicitWidth/2;
            }
        }
    }
}
