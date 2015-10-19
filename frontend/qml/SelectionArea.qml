import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Item {
    width: 100
    height: 6

//    DropShadow {
//        source: selectionArea
//        anchors.fill: source
//        transparentBorder: true
//        horizontalOffset: 8
//        radius: 32
//        samples: 32
//        opacity: 0.5
//    }

    Rectangle  {
        id: selectionArea;
        anchors.fill: parent;
        color: PhxTheme.common.primaryBackgroundColor;

        /* Rectangle {
            anchors { right: parent.right; rightMargin: -1; }
            width: 1;
            height: parent.height;
            color: Qt.rgba(255,255,255,.1);
        } */

        Row { anchors { top: parent.top; bottom: parent.bottom; right: parent.right; } }

        ColumnLayout {
            id: selectionColumnLayout;
            anchors.fill: parent;
            spacing: 0;

            // The main attraction: a StackView that holds a list of something, whether that something is a type of
            // console, settings category, or whatever else
            StackView {
                id: sectionsAreaStackView;
                Layout.fillHeight: true;
                Layout.fillWidth: true;
                initialItem: platformsView;

                Component {
                    id: platformsView;
                    PlatformsView { objectName: "PlatformsView"; }
                }

                Component {
                    id: favoritesView;
                    CollectionsView { objectName: "CollectionsView"; }
                }

                Component {
                    id: settingsView;
                    SettingsView { objectName: "SettingsView"; }
                }

                delegate: StackViewDelegate {
                    function transitionFinished(properties) {
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
                            easing.type: Easing.InOutExpo
                        }

                        PropertyAnimation {
                            target: exitItem
                            property: "y"
                            from: 0;
                            to: exitItem.height;
                            easing.type: Easing.InOutExpo
                        }
                    }
                }
            }

            // The buttons along the bottom that control the sectionsAreaStackView
            ListView {
                id: selectionAreaToolbar;
                height: 65;
                anchors { left: parent.left; right: parent.right; }
                spacing: 0;
                interactive: false;
                orientation: ListView.Horizontal;

                model: ListModel {
                    ListElement { bgColor: "white"; label: "Games"; imageSource: "games.svg"; }
                    ListElement { bgColor: "white"; label: "Favorites"; imageSource: "collections.svg"; }
                    ListElement { bgColor: "white"; label: "Settings"; imageSource: "settings.svg"; }
                    ListElement { bgColor: "white"; label: "Add Games"; imageSource: "add.svg"; }
                }

                FileDialog {
                    id: fileDialog;
                    selectFolder: true;
                    onAccepted: { contentArea.contentLibraryModel.append( fileUrl ); }
                }

                delegate: Item {
                    height: parent.height;
                    width: selectionAreaToolbar.width / selectionAreaToolbar.count;

                    Row { anchors { right: parent.right; bottom: parent.bottom; bottomMargin: 6; } }

                    Image {
                        height: 24; width: height;
                        source: imageSource;
                        sourceSize { height: height; width: width; }
                        fillMode: Image.PreserveAspectFit;
                        smooth: false;
                        anchors { centerIn: parent; }
                    }

                    Text {
                        visible: imageSource === "";
                        anchors.centerIn: parent;
                        text: label;
                        color: PhxTheme.common.baseFontColor;
                        font { pointSize: PhxTheme.selectionArea.basePixelSize; }
                    }

                    MouseArea {
                    // NativeTooltip {
                        anchors.fill: parent;
                        // text: label;
                        onClicked: {
                            switch( index ) {
                            case 0:
                                if ( sectionsAreaStackView.currentItem.objectName !== "PlatformsView" ) {
                                    sectionsAreaStackView.push( { item: platformsView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "BoxartGridView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentBoxartGrid, replace: true } );
                                    }
                                }
                                break;

                            case 1:
                                if ( sectionsAreaStackView.currentItem.objectName !== "CollectionsView" ) {
                                    sectionsAreaStackView.push( { item: favoritesView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "BoxartGridView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentBoxartGrid, replace: true } );
                                    }
                                }
                                break;

                            case 2:                                
                                if ( sectionsAreaStackView.currentItem.objectName !== "SettingsView" ) {
                                    sectionsAreaStackView.push( { item: settingsView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "InputView") {
                                        // contentArea.contentStackView.push( { item: contentArea.contentInputView, replace: true } );
                                    }
                                }
                                break;

                            case 3:
                                fileDialog.open();
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
