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

    // FIXME: For testing
    // Toggle pause
    Shortcut {
        autoRepeat: false;
        sequence: "g";
        onActivated: {
            if( controlOutput.state === Node.Playing ) {
                gameConsole.pause();
            } else if( controlOutput.state === Node.Paused ) {
                gameConsole.play();
            }
        }
    }

    // FIXME: For testing
    // Stop the running game
    Shortcut {
        autoRepeat: false;
        sequence: "h";
        onActivated: {
            if( controlOutput.state === Node.Playing || controlOutput.state === Node.Paused ) {
                gameConsole.stop();
            }
        }
    }

    // FIXME: For testing
    // Reload the game
    Shortcut {
        autoRepeat: false;
        sequence: "t";
        onActivated: {
            if( controlOutput.state === Node.Stopped ) {
                gameConsole.play();
            }
        }
    }

    // Elements of the Phoenix UI

    // A flexible multi-system emulator controlled via gameConsole, providing state updates via controlOutput
    // and controller input data via globalGamepad
    Emulator {
        id: emulator;
        anchors.fill: parent;

        // FIXME: Cleaner way to do this
        Component.onCompleted: {
            emulator.gameConsole.variableModel = window.libretroVariableModel;
            emulator.gameConsole.remapperModel = remapperModel;
        }
    }

    // Make Emulator and its most important children available globally
    property alias emulator: emulator;
    property alias controlOutput: emulator.controlOutput;
    property alias gameConsole: emulator.gameConsole;
    property alias globalGamepad: emulator.globalGamepad;
    property alias videoOutput: emulator.videoOutput;

    // The user's game library
    Library {
        id: library;
        anchors.fill: parent;

        // Accept input only if we are stopped
        enabled: controlOutput.state === Node.Stopped;
        opacity: enabled ? 1.0 : 0.0;

        Behavior on opacity {
            NumberAnimation { duration: 500; }
        }
    }

    // Make Library (and (TODO) its most important children) available globally
    property alias library: library;

    // Settings
    // TODO: Move to Settings.qml

    property alias remapperModel: remapperModel;
    RemapperModel {
        id: remapperModel;
    }

    property alias libretroVariableModel: libretroVariableModel;
    LibretroVariableModel {
        id: libretroVariableModel;
    }

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


    // FIXME: For testing
    Shortcut {
        autoRepeat: false;
        sequence: "f";
        onActivated: toggleFullscreen();
    }

    function toggleFullscreen() {
        if( window.visibility === Window.FullScreen )
            window.visibility = Window.Windowed;
        else if( window.visibility & ( Window.Windowed | Window.Maximized ) )
            window.visibility = Window.FullScreen;
    }
}
