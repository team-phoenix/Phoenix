import QtQuick 2.3
import QtQuick.Layouts 1.1

import vg.phoenix.models 1.0


Rectangle {
    id: contentArea;
    height: 200;
    width: 200;
    color: "gray";

    property alias contentLibraryModel: libraryModel;

    HeaderBar {
        id: headerBar;
        z: 100;
        height: 36;
        state: "SELECTION";

        anchors {
            left: parent.left
            right: parent.right;
            top: parent.top;

            topMargin: 24;
            leftMargin: 12;
            rightMargin: 12;
        }



        /*MouseArea {
            anchors.fill: parent;
            onClicked: headerBar.state = "PLAYING_GAME";
        }
        */
    }

    LibraryModel {
        id: libraryModel;

        Component.onCompleted: libraryModel.startWorkerThread();
    }

    BoxartGrid {
        id: boxartGrid;
        anchors {
            fill: parent;
        }
        color: "gray";
    }

}
