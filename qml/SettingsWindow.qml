import QtQuick 2.3
import QtQuick.Controls 1.1

ApplicationWindow {
    id: settingsWindow;
    height: 800;
    width: 600;
    title: "Settings";

    //flags: "FramelessWindowHint";

    property alias video: videoSettings;
    property alias audio: audioSettings;
    property alias input: inputSettings;
    property alias advanced: advancedSettings;
    property alias save: saveSettings;
    property alias core: coreSettings;
    property alias library: librarySettings;
    property alias stack: stackView;

    onVisibleChanged: {
        if (visible) {
            var name = stackView.currentItem.name;
            settingsDropDown.visible = false;
            if (name === "video") {
                settingsWindow.height = 400;
                settingsWindow.width = 250;
            }
            else {
                settingsWindow.height = 600;
                settingsWindow.width = 400;
            }
        }
    }


    Rectangle {
        id: settingsRect;
        anchors.fill: parent;

        gradient: Gradient {
            GradientStop {position: 0.0; color: "#323232";}
            GradientStop {position: 1.0; color: "#272727";}

        }

        border {
            width: 1;
            color: "#0b0b0b";
        }

        Rectangle {
            anchors {
                top: parent.top;
                topMargin: settingsRect.border.width;
                left: parent.left;
                leftMargin: settingsRect.border.width;
                right: parent.right;
                rightMargin: settingsRect.border.width;

            }
            height: 1;
            color: "#4d4d4d";
        }

        Rectangle {
            anchors {
                left: parent.left;
                leftMargin: settingsRect.border.width;
                top: parent.top;
                topMargin: settingsRect.border.width + 1;
                bottom: parent.bottom;
                bottomMargin: settingsRect.border.width + 1;
            }
            color: "#383838";
            width: 1
        }

        Rectangle {
            anchors {
                right: parent.right;
                rightMargin: settingsRect.border.width;
                top: parent.top;
                topMargin: settingsRect.border.width + 1;
                bottom: parent.bottom;
                bottomMargin: settingsRect.border.width + 1;
            }
            color: "#383838";
            width: 1
        }

        Rectangle {
            anchors {
                bottom: parent.bottom;
                bottomMargin: settingsRect.border.width;
                left: parent.left;
                right: parent.right;
                rightMargin: settingsRect.border.width;
                leftMargin: settingsRect.border.width;

            }
            height: 1;
            color: "#2b2b2b";
        }

        StackView {
            id: stackView;
            anchors.fill: parent;
            initialItem: videoSettings;
        }

        Component {
            id: audioSettings;
            AudioSettings {
                property string name: "audio";

            }
        }

        Component {
            id: videoSettings;
            VideoSettings {
                property string name: "video";
            }
        }

        Component {
            id: inputSettings;
            InputSettings {
                property string name: "input";

            }
        }

        Component {
            id: advancedSettings;
            AdvancedSettings {
                property string name: "advanced";

            }
        }

        Component {
            id: coreSettings;
            CoreSettings {
                property string name: "core";

            }
        }


        Component {
            id: librarySettings;
            LibrarySettings {
                property string name: "library";

            }
        }

        Component {
            id: saveSettings;
            SaveSettings {
                property string name: "save";

            }
        }
    }
}
