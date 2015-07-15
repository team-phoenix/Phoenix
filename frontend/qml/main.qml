import QtQuick 2.4
import QtQuick.Controls 1.3
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

    minimumHeight: 480;
    minimumWidth: 640;

    DropArea {
        id: rootDropArea;
        anchors.fill: parent;
    }

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

            width: 275;
            z: contentArea.z + 1;
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
