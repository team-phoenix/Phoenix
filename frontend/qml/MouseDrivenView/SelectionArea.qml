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
                highlightFollowsCurrentItem: false;

                signal doShowAnimation();

                highlight: Rectangle {
                    id: highlighter;
                    height: 4;
                    width: selectionAreaToolbar.currentItem.width;
                    color: PhxTheme.common.menuItemHighlight;

                    x: 0;
                    y: selectionAreaToolbar.currentItem.height - 4;

                    Connections {
                        target: selectionAreaToolbar;
                        onDoShowAnimation: {
                            showAnimation.complete();
                            showAnimation.start();
                        }
                    }

                    SequentialAnimation {
                        id: showAnimation;
                        PropertyAction { target: highlighter; properties: "x"; value: selectionAreaToolbar.currentItem.x; }
                        PropertyAnimation { target: highlighter; properties: "y"; from: selectionAreaToolbar.currentItem.height;
                            to: selectionAreaToolbar.currentItem.height - 4; duration: 300; easing.type: Easing.InOutQuart; }
                    }
                }

                model: ListModel {
                    ListElement { bgColor: "white"; label: "Games"; imageSource: "games.svg"; }
                    ListElement { bgColor: "white"; label: "Favorites"; imageSource: "collections.svg"; }
                    ListElement { bgColor: "white"; label: "Settings"; imageSource: "settings.svg"; }
                    ListElement { bgColor: "white"; label: "Add Games"; imageSource: "add.svg"; }
                }

                FileDialog {
                    id: fileDialog;
                    selectFolder: true;
                    onAccepted: { contentArea.contentLibraryModel.scanFolder( fileUrl ); }
                }

                delegate: Item {
                    height: parent.height;
                    width: selectionAreaToolbar.width / selectionAreaToolbar.count;

                    Image {
                        id: image;
                        anchors { centerIn: parent; }
                        width: 24;
                        height: 24;

                        source: imageSource;
                        sourceSize { width: width; height: height; }

                        opacity: index === selectionAreaToolbar.currentIndex ? 1.0 : 0.5;
                    }

                    Text {
                        visible: imageSource === "";
                        anchors.centerIn: parent;
                        text: label;
                        color: PhxTheme.common.baseFontColor;
                        font { pixelSize: PhxTheme.common.baseFontSize; }
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
                                selectionAreaToolbar.currentIndex = index;
                                selectionAreaToolbar.doShowAnimation();
                                break;

                            case 1:
                                if ( sectionsAreaStackView.currentItem.objectName !== "CollectionsView" ) {
                                    sectionsAreaStackView.push( { item: favoritesView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "BoxartGridView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentBoxartGrid, replace: true } );
                                    }
                                }
                                selectionAreaToolbar.currentIndex = index;
                                selectionAreaToolbar.doShowAnimation();
                                break;

                            case 2:                                
                                if ( sectionsAreaStackView.currentItem.objectName !== "SettingsView" ) {
                                    sectionsAreaStackView.push( { item: settingsView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "LibrarySettingsView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentLibrarySettingsView, replace: true } );
                                    }
                                }
                                selectionAreaToolbar.currentIndex = index;
                                selectionAreaToolbar.doShowAnimation();
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
