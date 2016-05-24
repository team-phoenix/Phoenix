import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

import "../Frontend"

Item {
    MouseArea {
        anchors.fill: parent;

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

            Library {
                id: library;
                color: PhxTheme.common.secondaryBackgroundColor;
                anchors { top: parent.top; bottom: parent.bottom; }
                Layout.fillWidth: true;
            }
        }

        onClicked: console.log( "Clicked" );

        // Wrap MinimizedGame with an Item so we can squash the Item without resizing MinimizedGame's contents
        Item {
            id: minimizedGame;
            property bool showMinimizedGame: phoenix.state === "Minimizing" || phoenix.state === "Minimized";
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
            height: showMinimizedGame ? 65 : 0;
            clip: true;
            Behavior on height { PropertyAnimation { duration: 250; } }

            MinimizedGame {
                anchors { top: parent.top; left: parent.left; right: parent.right; }
                height: 65;
            }
        }
    }
}
