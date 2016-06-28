import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import Phoenix.Backend 1.0
import Phoenix.Launcher 1.0
import Phoenix.Models 1.0
import Phoenix.Paths 1.0
import Phoenix.Scanner 1.0

import Phoenix.Emulator 1.0
import Phoenix.Settings 1.0
import Phoenix.Theme 1.0
import Phoenix.Util 1.0

Item {
    property alias libraryModel: libraryModel;

    SqlThreadedModel {
        id: libraryModel;

        databaseSettings {
            connectionName: "USERDATA";
        }

        fileLocation: PhxPaths.qmlUserDataLocation() + "/userdata.sqlite";

        autoCreate: true;

        tableName: "games";

        SqlColumn { name: "rowIndex"; type: "INTEGER PRIMARY KEY AUTOINCREMENT"; }
        SqlColumn { name: "title"; type: "TEXT NOT NULL"; }
        SqlColumn { name: "system"; type: "TEXT"; }
        SqlColumn { name: "region"; type: "TEXT"; }
        SqlColumn { name: "goodtoolsCode"; type: "TEXT"; }
        SqlColumn { name: "timePlayed"; type: "DATETIME"; }
        SqlColumn { name: "artworkUrl"; type: "TEXT"; }
        SqlColumn { name: "coreFilePath"; type: "TEXT"; }
        SqlColumn { name: "absolutePath"; type: "TEXT"; }
        SqlColumn { name: "absoluteFilePath"; type: "TEXT UNIQUE NOT NULL"; }
        SqlColumn { name: "crc32Checksum"; type: "TEXT"; }

        Component.onCompleted: {
            libraryModel.setOrderBy( "title", SqlModel.ASC );
            libraryModel.finishModelConstruction();
            GameHasherController.scanCompleted.connect( libraryModel.addEntries );
        }
    }

    // Used to get default cores and to verify that files exist before launch
    GameLauncher {
        id: gameLauncher;
    }

    GameGrid {
        anchors.fill: parent;
    }
}
