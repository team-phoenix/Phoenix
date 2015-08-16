import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Item {
    width: 100
    height: 6

    DropShadow {
        anchors.fill: source;
        source: selectionArea;
        transparentBorder: true;
        verticalOffset: 0;
        horizontalOffset: 1;
        color: "#b0000000";
        radius: 8.0;
        samples: radius * 2;
    }

    Rectangle  {
        id: selectionArea;
        anchors.fill: parent;

        /*
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#59070d"; }
            GradientStop { position: 0.7; color: "#24000c"; }
            GradientStop { position: 1.0; color: "#24000c"; }
        }
        */

        color: "#241312";

        ColumnLayout {
            anchors.fill: parent;
            spacing: 0;

            Rectangle {
                height: 0;
                Layout.fillWidth: true;
                color: "orange";

            }

            StackView {
                id: sectionsAreaStackView;
                Layout.fillHeight: true;
                Layout.fillWidth: true;

                initialItem: platformsView;

                Component {
                    id: platformsView;
                    PlatformsView {
                        objectName: "PlatformsView";
                    }
                }

                Component {
                    id: favoritesView;

                    CollectionsView {
                        objectName: "CollectionsView";
                    }
                }

                Component {
                    id: settingsView;


                    SettingsView {
                        objectName: "SettingsView";
                    }
                }

                delegate: StackViewDelegate {
                    function transitionFinished(properties)
                    {
                        properties.exitItem.opacity = 1
                        properties.exitItem.y = 0;
                    }

                    pushTransition: StackViewTransition {
                        PropertyAnimation {
                            target: enterItem
                            property: "opacity"
                            from: 0
                            to: 1
                        }
                        PropertyAnimation {
                            target: exitItem
                            property: "opacity"
                            from: 1
                            to: 0
                        }
                        PropertyAnimation {
                            target: enterItem
                            property: "y"
                            from: enterItem.height;
                            to: 0;
                        }

                        PropertyAnimation {
                            target: exitItem
                            property: "y"
                            from: 0;
                            to: exitItem.height;
                        }
                    }
                }
            }

            ListView {
                id: selectionAreaToolbar;
                height: 62;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }

                spacing: 0;
                interactive: false;
                orientation: ListView.Horizontal;

                model: ListModel {
                    ListElement { bgColor: "black"; label: "Add Games"; imageSource: ""; }
                    ListElement { bgColor: "black"; label: "Favorites"; imageSource: "collections.svg"; }
                    ListElement { bgColor: "black"; label: "Games"; imageSource: ""; }
                    ListElement { bgColor: "black"; label: "Settings"; imageSource: "settings.svg"; }
                }

                FileDialog {
                    id: fileDialog;
                    selectFolder: true;
                    onAccepted: {
                        contentArea.contentLibraryModel.append( fileUrl );
                    }
                }

                /*
                header: Rectangle {
                    width: selectionAreaToolbar.width;

                    height: 2;
                    color: "white";
                    opacity: 0.15;
                }
                */

                delegate: Rectangle {
                    height: parent.height;
                    width: selectionAreaToolbar.width / selectionAreaToolbar.count;
                    color: "white" //bgColor;


                    Image {
                        sourceSize {
                            height: 24;
                            width: 24;
                        }

                        source: imageSource;
                        anchors {
                            centerIn: parent;
                        }
                    }

                    Text {
                        visible: imageSource === "";
                        anchors.centerIn: parent;
                        text: label;

                        font {
                            pixelSize: PhxTheme.selectionArea.basePixelSize;
                        }

                        color: PhxTheme.common.baseFontColor;
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            switch( index ) {
                            case 0:
                                fileDialog.open();
                                break;
                            case 1:
                                if ( sectionsAreaStackView.currentItem.objectName !== "CollectionsView" ) {
                                    sectionsAreaStackView.push( { item: favoritesView, replace: true } );
                                }
                                break;
                            case 2:
                                if ( sectionsAreaStackView.currentItem.objectName !== "PlatformsView" ) {
                                    sectionsAreaStackView.push( { item: platformsView, replace: true } );
                                }
                                break;
                            case 3:
                                if ( sectionsAreaStackView.currentItem.objectName !== "SettingsView" ) {
                                    sectionsAreaStackView.push( { item: settingsView, replace: true } );
                                }
                                break;
                            default:
                                break;
                            }

                        }
                    }
                }
            }
        }
    }
}

