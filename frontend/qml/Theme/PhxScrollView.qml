import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0

ScrollView {
    style: ScrollViewStyle {

        scrollBarBackground: Item { width: 9; }
        incrementControl: Item { }
        decrementControl: Item { }

        handle: Item {
            implicitWidth: 8;
            implicitHeight: 30;
            x: 1;

            Rectangle {
                id: handleRectangle;
                color: "#20FFFFFF";
                anchors.fill: parent;
                radius: parent.implicitWidth/2;
            }
        }
    }
}
