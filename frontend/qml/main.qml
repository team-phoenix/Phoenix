import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import vg.phoenix.backend 1.0

ApplicationWindow {
    id: root;
    visible: true
    width: Screen.width * 0.7;
    height: Screen.height * 0.7;
    title: qsTr("Phoenix");
    color: "black";

    property color normalFontColor: "#f1f1f1";

    minimumHeight: 480;
    minimumWidth: 640;

    RowLayout {
        id: gameSelectionPane;
        anchors {
            fill: parent;
        }

        spacing: 0;

        SelectionArea {
            id: selectionArea;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            width: 250;
        }


        ContentArea {
            id: contentArea;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }

            Layout.fillWidth: true;

        }
    }


}
