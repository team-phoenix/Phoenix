import QtQuick 2.5
import QtQuick.Controls 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Item {
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

                property string currentObjectName: currentItem === null ? "" : currentItem.objectName;

                PlatformsView {
                    id: platformsView;
                    objectName: "PlatformsView";
                    visible: !sectionsAreaStackView.currentObjectName.localeCompare( objectName );
                    enabled: visible;
                }

                CollectionsView {
                    id: collectionsView;
                    objectName: "CollectionsView";
                    visible: !sectionsAreaStackView.currentObjectName.localeCompare( objectName );
                    enabled: visible;
                }

                SettingsView {
                    id: settingsView;
                    objectName: "SettingsView";
                    visible: !sectionsAreaStackView.currentObjectName.localeCompare( objectName );
                    enabled: visible;
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

            Item {
                id: bottomRowContainer;
                height: 65;
                anchors { left: parent.left; right: parent.right; }

                // Added number: number of extra buttons past ListView
                property real delegateWidth: width / ( listView.count + 1 );

                // The buttons along the bottom that control the sectionsAreaStackView
                // @disable-check M300
                PhxListView {
                    id: listView;
                    anchors { top: parent.top; bottom: parent.bottom; left: parent.left; }
                    width: parent.width - parent.delegateWidth;
                    spacing: 0;
                    orientation: ListView.Horizontal;

                    model: ListModel {
                        ListElement { bgColor: "white"; label: "Games"; imageSource: "games.svg";
                            leftPane: "PlatformsView"; rightPane: "BoxartGridView"; }
                        ListElement { bgColor: "white"; label: "Favorites"; imageSource: "collections.svg";
                            leftPane: "CollectionsView"; rightPane: "BoxartGridView"; }
                        ListElement { bgColor: "white"; label: "Settings"; imageSource: "settings.svg";
                            leftPane: "SettingsView"; rightPane: "LibrarySettingsView"; }
                    }

                    FileDialog {
                        id: fileDialog;
                        selectFolder: true;
                        onAccepted: { contentArea.contentLibraryModel.scanFolder( fileUrl ); }
                    }

                    // Returns first match
                    function getObjectByName( name, parentObject ) {
                        for( var i = 0; i < parentObject.children.length; i++ ) {
                            if( !parentObject.children[ i ].objectName.localeCompare( name ) )
                                return parentObject.children[ i ];
                        }
                        return null;
                    }

                    // Change the selectionArea and contentArea stack views if they're not already set to the given objects
                    function changePlaces( leftPane, rightPane ) {

                        // Happens during startup/shutdown
                        if( sectionsAreaStackView.currentItem === null ) return;

                        if ( sectionsAreaStackView.currentItem.objectName.localeCompare( leftPane ) ) {
                            sectionsAreaStackView.push( getObjectByName( leftPane, sectionsAreaStackView ) );
                            if( contentArea.contentStackView.currentItem.objectName.localeCompare( rightPane ) ) {
                                contentArea.contentStackView.push( getObjectByName( rightPane, contentArea.contentStackView ) );
                            }
                        }
                    }

                    delegate: Item {
                        anchors { top: parent.top; bottom: parent.bottom; }
                        width: bottomRowContainer.delegateWidth;

                        Image {
                            anchors { centerIn: parent; }
                            width: 24;
                            height: 24;

                            source: imageSource;
                            sourceSize { width: width; height: height; }

                            opacity: index === listView.currentIndex ? 1.0 : 0.5;
                        }

                        // If the index now points to this particular delegate, manipulate the stackview to match its role
                        Connections {
                            target: listView;
                            onCurrentIndexChanged: {
                                if( listView.currentIndex === index ) {
                                    listView.changePlaces( leftPane, rightPane );
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: listView.currentIndex = index;
                        }
                    }
                }

                Rectangle {
                    width: parent.delegateWidth;
                    anchors { top: parent.top; bottom: parent.bottom; left: listView.right; }

                    color: "transparent";
                    border.color: activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
                    border.width: 2;

                    activeFocusOnTab: true;

                    Image {
                        id: addGameButton;
                        anchors { centerIn: parent; }
                        width: 24;
                        height: 24;

                        source: "add.svg";
                        sourceSize { width: width; height: height; }

                        opacity: 0.5;
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: fileDialog.open();
                    }
                }
            }
        }
    }
}
