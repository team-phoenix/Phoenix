import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

ApplicationWindow {
    id: root;
    visible: true;
    visibility: Window.Windowed;
    width: Screen.width * 0.7;
    height: Screen.height * 0.7;
    color: "black";

    property int defaultMinHeight: 600;
    property int defaultMinWidth: 900;
    minimumHeight: defaultMinHeight;
    minimumWidth: defaultMinWidth;

    GameLauncher {
        id: gameLauncher;
    }

    property InputManager inputManager: InputManager {
        id: inputManager;
        gamepadControlsFrontend: true;
    }

    property var gameViewObject: null;

    function resetTitle() { title = ""; }

    property alias layoutStackView: layoutStackView;
    StackView {
        id: layoutStackView;
        anchors.fill: parent;

        // Slightly counterintuitive, but necessary as a transition immediately occurs upon launch
        property bool transitioning: true;

        Component.onCompleted: {
            root.disableMouseClicks();
            root.gameViewObject = push( { item: gameView } );
            push( { item: mouseDrivenView, properties: { opacity: 0 } } );
        }

        // Here we define the (push) transition that occurs when going from GameView into the library
        Component {
            id: libraryTransition;
            StackViewTransition {
                PropertyAction {
                    target: layoutStackView;
                    property: "transitioning";
                    value: true;
                }
                PropertyAnimation {
                    target: exitItem; property: "opacity";
                    from: 1; to: 0;
                    duration: 1000;
                }

                PropertyAnimation {
                    target: exitItem; property: "scale";
                    from: 1; to: 0.75;
                    duration: 1000;
                    easing.type: Easing.InOutQuad;
                }

                PropertyAnimation {
                    target: enterItem; property: "opacity";
                    from: 0; to: 1;
                    duration: 1000;
                }
            }
        }

        // Here we define the opposite (pop) transition
        Component {
            id: gameTransition;
            StackViewTransition {
                PropertyAction {
                    target: layoutStackView;
                    property: "transitioning";
                    value: true;
                }

                PropertyAnimation {
                    target: exitItem; property: "opacity";
                    from: 1; to: 0;
                    duration: 500;
                }

                PropertyAnimation {
                    target: enterItem; property: "opacity";
                    from: 0; to: 1;
                    duration: 1000;
                    easing.type: Easing.InOutQuad;
                }

                PropertyAnimation {
                    target: enterItem; property: "scale";
                    from: 0.75; to: 1;
                    duration: 1000;
                    easing.type: Easing.InOutQuad;
                }
            }
        }

        delegate: StackViewDelegate {
            function transitionFinished(){
                console.log( "transitionFinished()" );
                rootMouseArea.cursorShape = Qt.ArrowCursor;
                root.enableMouseClicks();

                // Enable hover events iff GameView is the current top of the stack
                if( layoutStackView.depth === 1 ) { rootMouseArea.hoverEnabled = true; }
                layoutStackView.transitioning = false;
            }

            pushTransition: libraryTransition;
            popTransition: gameTransition;
        }
    }

    // Use when transitioning
    function disableMouseClicks() {
        rootMouseArea.propagateComposedEvents = false;
        rootMouseArea.acceptedButtons = Qt.AllButtons;
    }
    function enableMouseClicks()  {
        rootMouseArea.propagateComposedEvents = true;
        rootMouseArea.acceptedButtons = Qt.NoButton;
    }

    property alias gameActionBarMouseArea: gameActionBarMouseArea;
    property bool touchMode: false;
    MouseArea {
        id: rootMouseArea;
        anchors.fill: parent;
        hoverEnabled: false;

        // Let clicks reach everything below this
        propagateComposedEvents: true;
        acceptedButtons: Qt.NoButton;

        MouseArea {
            id: gameActionBarMouseArea;
            anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom; bottomMargin: 10; }
            width: 350;
            height: 45;
            visible: !touchMode && layoutStackView.depth === 1 && !layoutStackView.transitioning;
            enabled: visible;
            hoverEnabled: true;
            preventStealing: true;
            acceptedButtons: Qt.NoButton;
        }
    }

    Component {
        id: bigPictureView;
        BigPictureView {
            id: pictureView;
            objectName: "BigPictureView";

            Connections {
                target: root.inputManager;
                onDeviceAdded: {
                    console.log( device.name );
                    console.log( device.editMode )
                    device.inputDeviceEvent.connect( pictureView.qmlInputDevice.insert );
                }
            }

            Component.onCompleted: { root.inputManager.emitConnectedDevices(); }
        }
    }

    Component {
        id: mouseDrivenView;
        MouseDrivenView { objectName: "MouseDrivenView"; }
    }

    Component {
        id: gameView;
        GameView { objectName: "GameView"; }
    }
}
