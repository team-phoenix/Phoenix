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
import "../Util"

// Main window
PhoenixWindow {
    id: window;
    visible: true;

    Component.onCompleted: {
        // Centers the window with a single shot assignment
        // while still allowing for resize.
        x = Screen.width / 2 - width / 2;
        y = Screen.height / 2 - height / 2;
    }

    width: Screen.width * 0.7;
    height: Screen.height * 0.7;
    color: "black";

   /*
    * Phoenix's QML state machine
    * Controls:
    *   - Visibility of the library and emulator
    *   - When either the library or emulator should accept mouse/keyboard input
    *   - When the Phoenix logo animation should play
    *   - GameConsole via function calls
    *   - The cursor type (except when Playing or Paused)
    *
    * Note: Remember to either set explicit to true or use a StateChangeScript for property changes unless you want
    *       to interpolate them
    */
    StateGroup {
        id: phoenix;

        // Intiailly we should show the library view
        Component.onCompleted: {
            if( state === "" ) {
                state = "Stopped";
            }
            phoenixLogoAnimation.restart();
        }

        onStateChanged: {
            //console.log( "QML state changed to " + state );
            //console.trace();
        }

        // All possible states
        // Valid state transitions are given by "Next:"
        states: [
            // Nothing loaded, in the library view
            // Next: Loading
            State {
                name: "Stopped";

                PropertyChanges { target: library; enabled: true; explicit: true; }
                PropertyChanges { target: library; opacity: 1.0; scale: 1.0; }
                PropertyChanges { target: logoEffect; opacity: 0.0; scale: 0.5; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.ArrowCursor; }
                PropertyChanges { target: window; title: "Library"; explicit: true; }
            },

            // A game has been selected, begin the load
            // Next: Playing, Paused, Stopped
            // TODO: Load error state?
            State {
                name: "Loading";

                PropertyChanges { target: emulator; scale: 1.0; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BusyCursor; }
                PropertyChanges { target: window; title: "Loading - " + emulator.title; explicit: true; }

                StateChangeScript { script: gameConsole.play(); }
                StateChangeScript { script: phoenixLogoAnimation.restart(); }
            },

            // Emulation active
            // Next: Paused, Minimizing, Unloading
            State {
                name: "Playing";

                PropertyChanges { target: emulator; enabled: true; explicit: true; }
                PropertyChanges { target: emulator; scale: 1.0; }
                PropertyChanges { target: logoEffect; opacity: 0.0; scale: 0.5; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BlankCursor; }
                PropertyChanges { target: window; title: /*"Playing - " +*/  emulator.title; explicit: true; }

                StateChangeScript { script: gameConsole.play(); }
            },

            // Emulation paused
            // Next: Playing, Minimizing, Unloading
            State {
                name: "Paused";

                PropertyChanges { target: emulator; enabled: true; explicit: true; }
                PropertyChanges { target: emulator; scale: 1.0; }
                PropertyChanges { target: logoEffect; opacity: 0.0; scale: 0.5; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BlankCursor; }
                PropertyChanges { target: window; title: "Paused - " + emulator.title; explicit: true; }

                StateChangeScript { script: gameConsole.pause(); }
            },

            // Unload core, write save file, etc.
            // Next: Stopped
            State {
                name: "Unloading";

                PropertyChanges { target: emulator; scale: 1.0; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BusyCursor; }
                PropertyChanges { target: window; title: "Unloading - " + emulator.title; explicit: true; }

                StateChangeScript { script: gameConsole.stop(); }
                StateChangeScript { script: phoenixLogoAnimation.restart(); }
            },

            // Show the library without unloading the game
            // Next: Minimized
            State {
                name: "Minimizing";
                PropertyChanges { target: library; opacity: 1.0; scale: 1.0; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BusyCursor; }
                PropertyChanges { target: window; title: /*"Minimizing - " +*/ emulator.title; explicit: true; }

                StateChangeScript { script: phoenixLogoAnimation.restart(); }
            },

            // The library is shown and the game is paused
            // Next: Restoring, Unloading
            State {
                name: "Minimized";
                PropertyChanges { target: logoEffect; opacity: 0.0; scale: 0.5; }
                PropertyChanges { target: library; enabled: true; explicit: true; }
                PropertyChanges { target: library; opacity: 1.0; scale: 1.0; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.ArrowCursor; }
                PropertyChanges { target: window; title: "Minimized - " + emulator.title; explicit: true; }
            },

            // Hide the library and transition to paused
            // Next: Paused
            State {
                name: "Restoring";
                PropertyChanges { target: emulator; scale: 1.0; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BusyCursor; }
                PropertyChanges { target: window; title: /*"Restoring - " +*/ emulator.title; explicit: true; }

                StateChangeScript { script: phoenixLogoAnimation.restart(); }
            },

            // Unload the game without hiding the library
            // Next: Stopped
            State {
                name: "SilentlyUnloading";

                PropertyChanges { target: library; opacity: 1.0; }
                PropertyChanges { target: logoEffect; opacity: 0.0; scale: 0.5; }
                PropertyChanges { target: mouseArea; cursorShape: Qt.BusyCursor; }
                PropertyChanges { target: window; title: "Unloading - " + emulator.title; explicit: true; }

                StateChangeScript { script: gameConsole.stop(); }
            }
        ]

        property int transitionDuration: 333;
        //property string transitionProperties: "opacity,scale";

        // Interpolate property changes
        transitions: Transition {
            id: defaultTransition;
            PropertyAnimation { property: "opacity"; duration: phoenix.transitionDuration; }
            PropertyAnimation { property: "scale"; easing.type: Easing.InOutSine; duration: phoenix.transitionDuration; }
        }
    }

    // Automatic state transitions
    Item {
        // Loading -> Playing/Paused (depending on autoplay setting)
        // Both the animation and the load must be complete before transitioning
        Item {
            id: loadChecker;
            // Both flags must be true to transition from Loading to Playing/Paused
            property bool loadCompleteFlag: false;
            property bool animationCompleteFlag: false;

            // TODO: Let the user set this
            property bool autoplay: true;

            // Check the flags when either change
            onLoadCompleteFlagChanged: checkFlags();
            onAnimationCompleteFlagChanged: checkFlags();

            function checkFlags() {
                if( phoenix.state === "Loading" && loadChecker.loadCompleteFlag && loadChecker.animationCompleteFlag ) {
                    phoenix.state = loadChecker.autoplay ? "Playing" : "Paused";
                }
            }

            // Set the flags

            // Change to Playing if the animation's done and we're done loading
            Connections {
                target: phoenixLogoAnimation;
                onRunningChanged: { loadChecker.animationCompleteFlag = !phoenixLogoAnimation.running; }
            }

            // Set a flag if we're done loading but the animation isn't done
            Connections {
                target: controlOutput;
                onStateChanged: { loadChecker.loadCompleteFlag = controlOutput.state === Node.Paused; }
            }
        }

        // Unloading -> Stopped
        // Both the animation and the unload must be complete before transitioning
        Item {
            id: unloadChecker;
            // Both flags must be true to transition from Unloading to Stopped
            property bool unloadCompleteFlag: false;
            property bool animationCompleteFlag: false;

            // Check the flags when either change
            onUnloadCompleteFlagChanged: checkFlags();
            onAnimationCompleteFlagChanged: checkFlags();

            function checkFlags() {
                if( phoenix.state === "Unloading" && unloadChecker.unloadCompleteFlag && unloadChecker.animationCompleteFlag ) {
                    phoenix.state = "Stopped";
                }
            }

            // Set the flags

            // Change to Playing if the animation's done and we're done loading
            Connections {
                target: phoenixLogoAnimation;
                onRunningChanged: { unloadChecker.animationCompleteFlag = !phoenixLogoAnimation.running; }
            }

            // Set a flag if we're done loading but the animation isn't done
            Connections {
                target: controlOutput;
                onStateChanged: { unloadChecker.unloadCompleteFlag = controlOutput.state === Node.Stopped; }
            }
        }

        // Minimizing -> Minimized
        // Change to Minimized once the animation completes
        Connections {
            target: phoenixLogoAnimation;
            onRunningChanged: {
                if( phoenix.state === "Minimizing" && phoenixLogoAnimation.running === false ) {
                    phoenix.state = "Minimized";
                }
            }
        }

        // Restoring -> Paused
        // Change to Paused once the animation completes
        Connections {
            target: phoenixLogoAnimation;
            onRunningChanged: {
                if( phoenix.state === "Restoring" && phoenixLogoAnimation.running === false ) {
                    phoenix.state = "Paused";
                }
            }
        }

        // SilentlyUnloading -> Stopped
        // Let the user choose another game once the previous one unloads
        Connections {
            target: controlOutput;
            onStateChanged: {
                if( phoenix.state === "SilentlyUnloading" && controlOutput.state === Node.Stopped ) {
                    phoenix.state = "Stopped";
                }
            }
        }
    }

    // Elements of the Phoenix UI

    // The entire program is wrapped with a mouse area that allows us to globally set the mouse cursor and detect
    // mouse move events
    MouseArea {
        id: mouseArea;
        anchors.fill: parent;

        acceptedButtons: Qt.NoButton;
        hoverEnabled: true;

        // A flexible multi-system emulator controlled via gameConsole, providing state updates via controlOutput
        // and controller input data via globalGamepad
        Emulator {
            id: emulator;
            anchors.fill: parent;

            //opacity: 1.0;
            scale: 0.85;

            //onEnabledChanged: console.log( "Emulator enabled = " + enabled );
            enabled: false;

            // FIXME: Cleaner way to do this
            Component.onCompleted: {
                emulator.gameConsole.variableModel = window.libretroVariableModel;
                emulator.gameConsole.remapperModel = remapperModel;
            }
        }

        // The user's game library
        Library {
            id: library;
            anchors.fill: parent;

            opacity: 0.0;
            scale: 1.15;

            layer.enabled: true;

            //onEnabledChanged: console.log( "Library enabled = " + enabled );
            enabled: false;
        }

        // Settings
        // TODO: Move to Settings.qml
        Item {
            id: settings;
            RemapperModel {
                id: remapperModel;
            }
            LibretroVariableModel {
                id: libretroVariableModel;
            }
        }

        // A cool logo effect
        PhoenixLogo {
            id: logoEffect;
            anchors.centerIn: parent;
            width: 150;
            height: 150;
            // Default, must match the value set when this logo should be hidden (Stopped, Paused, Playing, Minimized)
            scale: 0.5;
            animationSpeed: phoenix.transitionDuration;
        }
    }
    // Make Emulator and its most important children available globally
    property alias emulator: emulator;
    property alias controlOutput: emulator.controlOutput;
    property alias gameConsole: emulator.gameConsole;
    property alias globalGamepad: emulator.globalGamepad;
    property alias videoOutput: emulator.videoOutput;

    // Make Library and its most important children available globally
    property alias library: library;

    // Make Settings and its most important children available globally
    property alias remapperModel: remapperModel;
    property alias libretroVariableModel: libretroVariableModel;

    // Make PhoenixLogo and its most important children available globally
    property alias phoenixLogo: logoEffect.phoenixLogo;
    property alias phoenixLogoAnimation: logoEffect.phoenixLogoAnimation;

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

    // Testing stuff
    // FIXME: Remove

    // FIXME: For testing
    Shortcut {
        autoRepeat: false;
        sequence: "f";
        onActivated: toggleFullscreen();
    }

    // FIXME: For testing
//    MouseArea{
//        anchors.fill: parent;
//        onDoubleClicked: toggleFullscreen();
//    }

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
            if( phoenix.state === "Playing" ) {
                phoenix.state = "Paused";
            } else if( phoenix.state === "Paused" ) {
                phoenix.state = "Playing";
            }
        }
    }

    // FIXME: For testing
    // Stop the running game
    Shortcut {
        autoRepeat: false;
        sequence: "h";
        onActivated: {
            if( phoenix.state === "Playing" || phoenix.state === "Paused" ) {
                phoenix.state = "Unloading";
            }

            if( phoenix.state === "Minimized" ) {
                phoenix.state = "SilentlyUnloading";
            }
        }
    }

    // FIXME: For testing
    // Load the game
    Shortcut {
        autoRepeat: false;
        sequence: "t";
        onActivated: {
            if( phoenix.state === "Stopped" || phoenix.state === "" ) {
                phoenix.state = "Loading";
            }
        }
    }


    // FIXME: For testing
    // Minimize
    Shortcut {
        autoRepeat: false;
        sequence: "y";
        onActivated: {
            // Pause if we're playing
            if( phoenix.state === "Playing" ) {
                phoenix.state = "Paused";
            }

            // We either just paused or we were already paused
            if( phoenix.state === "Paused" ) {
                phoenix.state = "Minimizing";
            }
        }
    }

    // FIXME: For testing
    // Restore
    Shortcut {
        autoRepeat: false;
        sequence: "j";
        onActivated: {
            if( phoenix.state === "Minimized" ) {
                phoenix.state = "Restoring";
            }
        }
    }
}
