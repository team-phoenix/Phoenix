import QtQuick 2.3
import QtQuick.Layouts 1.1

Rectangle {
    id: contentArea;
    height: 200;
    width: 200;
    color: "gray";

    BoxartGrid {
        id: boxartGrid;
        anchors {
            fill: parent;
        }
        color: "gray";
    }

}
