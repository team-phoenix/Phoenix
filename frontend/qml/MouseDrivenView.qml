import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import vg.phoenix.cache 1.0
import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Item {
    DropArea {
        id: rootDropArea;
        anchors.fill: parent;
    }

    ColumnLayout {
        anchors.fill: parent;
        Rectangle {
            anchors.fill: parent;
            color: PhxTheme.common.baseBackgroundColor;
            Image { smooth: true; source: PhxTheme.common.backgroundImage; fillMode: Image.PreserveAspectCrop; }
        }

        RowLayout {
            id: gameSelectionPane;
            anchors { fill: parent }
            spacing: 0;
            anchors.bottomMargin: root.gameViewObject.videoItem.coreState === Core.STATEPAUSED ? gameSuspendedArea.height : 0;

            SelectionArea {
                id: selectionArea;
                anchors { top: parent.top; bottom: parent.bottom }
                width: PhxTheme.common.menuWidth;
                z: contentArea.z + 1;
            }

            ContentArea {
                id: contentArea;
                anchors { top: parent.top; bottom: parent.bottom }
                Layout.fillWidth: true;
            }
        }

        GameSuspendedArea {
            id: gameSuspendedArea;
            objectName: "GameSuspendedArea";
            visible: root.gameViewObject.videoItem.coreState === Core.STATEPAUSED;
        }
    }
}
