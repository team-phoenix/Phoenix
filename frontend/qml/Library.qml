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

        RowLayout {
            anchors.fill: parent;
            spacing: 0;

            Sidebar {
                id: sidebar;
                anchors { top: parent.top; bottom: parent.bottom; }
                width: PhxTheme.common.menuWidth;
            }

            Rectangle {
                id: mainArea;
                color: PhxTheme.common.secondaryBackgroundColor;
                anchors { top: parent.top; bottom: parent.bottom; }
                Layout.fillWidth: true;
            }
        }

        onClicked: console.log( "Clicked" );

        MinimizedGame {
            // Make sure it appears during the transition, too
            visible: phoenix.state === "Minimized" || phoenix.state === "Minimizing";
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
            height: 65;
        }
    }
}
