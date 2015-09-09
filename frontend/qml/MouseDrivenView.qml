import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Item {

    DropArea {
        id: rootDropArea
        anchors.fill: parent
    }

    RowLayout {
        id: gameSelectionPane
        anchors { fill: parent }
        spacing: 0

        SelectionArea {
            id: selectionArea
            anchors { top: parent.top; bottom: parent.bottom }
            width: 250;
            z: contentArea.z + 1
        }

        ContentArea {
            id: contentArea
            anchors { top: parent.top; bottom: parent.bottom }
            Layout.fillWidth: true
        }
    }
}

