import QtQuick 2.5
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

import "../Emulator"
import "../Library"

// Main window
PhoenixWindow {
    id: window;
    visible: true;
    x: Screen.width / 2 - width / 2;
    y: Screen.height / 2 - height / 2;
    width: Screen.width * 0.7;
    height: Screen.height * 0.7;
    color: "black";

    // FIXME: For testing
    MouseArea{
        anchors.fill: parent;
        onDoubleClicked: toggleFullscreen();
    }

    // FIXME: For testing
    Shortcut {
        autoRepeat: false;
        sequence: "v";
        onActivated: gameConsole.vsync = gameConsole.vsync ? false : true;
    }

    // Elements of the Phoenix UI

    Emulator {
        id: emulator;
        anchors.fill: parent;
    }

    // Make Emulator and its most important children available globally
    property alias emulator: emulator;
    property alias controlOutput: emulator.controlOutput;
    property alias gameConsole: emulator.gameConsole;
    property alias globalGamepad: emulator.globalGamepad;
    property alias videoOutput: emulator.videoOutput;

    Library {
        id: library;
    }
    // Make Library (and (TODO) its most important children) available globally
    property alias library: library;

    // Misc Window stuff

    // Fullscreen shortcuts
    Shortcut {
        autoRepeat: false;
        sequence: StandardKey.FullScreen;
        onActivated: toggleFullscreen();
    }

    Shortcut {
        autoRepeat: false;
        sequence: "Alt+Return";
        onActivated: toggleFullscreen();
    }

    function toggleFullscreen() {
        if( window.visibility === Window.FullScreen )
            window.visibility = Window.Windowed;
        else if( window.visibility & ( Window.Windowed | Window.Maximized ) )
            window.visibility = Window.FullScreen;
    }
}
