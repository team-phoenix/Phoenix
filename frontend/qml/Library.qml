import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

Item {
    MouseArea {
        anchors.fill: parent;
        id: libraryContainer;

        RowLayout {
            anchors.top: parent.top;
            anchors.bottom: minimizedGame.top;
            width: parent.width;
            spacing: 0;

            Sidebar {
                id: sidebar;
                anchors { top: parent.top; bottom: parent.bottom; }
                width: PhxTheme.common.menuWidth;
                clip: true;
            }

            Rectangle {
                id: mainArea;
                color: PhxTheme.common.secondaryBackgroundColor;
                anchors { top: parent.top; bottom: parent.bottom; }
                Layout.fillWidth: true;
            }
        }

        onClicked: console.log( "Clicked" );
        property bool showMinimizedGame: phoenix.state === "Minimizing" || phoenix.state === "Minimized";

        Item {
            id: minimizedGame;
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
            height: libraryContainer.showMinimizedGame ? 65 : 0;
            Behavior on height { PropertyAnimation { duration: 250; } }

            MinimizedGame {
                anchors { top: parent.top; left: parent.left; right: parent.right; }
                height: 65;
            }
        }
    }
}
