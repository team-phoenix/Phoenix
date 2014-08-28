import QtQuick 2.3
import QtQuick.Controls 1.1

ApplicationWindow {
    id: settingsWindow;
    height: 800;
    width: 600;

    property alias video: videoSettings;
    property alias audio: audioSettings;
    property alias input: inputSettings;
    property alias advanced: advancedSettings;
    property alias save: saveSettings;
    property alias core: coreSettings;
    property alias library: librarySettings;
    property alias stack: stackView;
    flags: "Dialog";

    onVisibleChanged: {
        if (visible) {
            var name = stackView.currentItem.name;
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
