import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.2

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0
import vg.phoenix.scanner 1.0

Rectangle  {
    id: selectionArea;
    color: PhxTheme.common.primaryBackgroundColor;

    // The main attraction: a StackView that holds a list of something, whether that something is a type of
    // console, settings category, or whatever else
    StackView {
        id: sectionsAreaStackView;
        initialItem: platformsView;
        anchors { left: parent.left; right: parent.right; top: parent.top; bottom: bottomRowContainer.top; }

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
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; }
        height: bottomRowHeight + ( running ? scanProgressHeight : 0 );
        Behavior on height { NumberAnimation { easing.type: Easing.InOutExpo; duration: 1000; } }

        property int scanProgressHeight: 50;
        property int bottomRowHeight: 65;

        property bool running: GameHasherController.running;

        // A progress indicator for the game scanner
        Rectangle {
            anchors { left: parent.left; right: parent.right; }

            color: PhxTheme.common.tertiaryBackgroundColor;

            height: bottomRowContainer.scanProgressHeight;

            y: parent.running ? 0 : bottomRowContainer.scanProgressHeight;
            Behavior on y { NumberAnimation { easing.type: Easing.InOutExpo; duration: 1000; } }

            Column {
                anchors {
                    fill: parent;
                    topMargin: 9;
                }
                spacing: 6;

                Text {
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                    }
                    text: "Importing Games";
                    color: "white";
                }

                Row {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                    }

                    ProgressBar {
                        height: 6;
                        value: GameHasherController.progress;
                        minimumValue: 0;
                        maximumValue: 500;
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }

                        width: parent.width * 0.6;

                        //anchors { left: parent.left; right: parent.right; leftMargin: 12; rightMargin: 12; }

                        style: ProgressBarStyle {
                            background: Rectangle {
                                color: "white";
                                border.width: 0;
                            }
                            progress: Rectangle {
                                color: PhxTheme.common.menuItemHighlight;
                                border.width: 0;
                            }
                        }
                    }

                    Button {
                        text: GameHasherController.paused ? "resume" : "pause";
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        onClicked: GameHasherController.paused ? GameHasherController.resume() : GameHasherController.pause();
                    }

                    Button {
                        text: "cancel";
                        anchors {
                            verticalCenter: parent.verticalCenter;
                        }
                        onClicked: GameHasherController.cancel();
                    }
                }



            }
        }

        Rectangle {
            id: listViewContainer;
            height: bottomRowContainer.bottomRowHeight;
            color: PhxTheme.common.primaryBackgroundColor;
            anchors { left: parent.left; right: parent.right; bottom: parent.bottom; }

            // Added number: number of extra buttons past ListView
            property real delegateWidth: parent.width / ( listView.count + 1 );

            // The buttons along the bottom that control the sectionsAreaStackView
            // @disable-check M300
            PhxListView {
                id: listView;
                anchors { bottom: parent.bottom; left: parent.left; }
                width: parent.width - parent.delegateWidth;
                height: bottomRowContainer.bottomRowHeight;
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
                    width: listViewContainer.delegateWidth;

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
                anchors { top: listView.top; bottom: listView.bottom; left: listView.right; }

                color: "transparent";
                border.color: activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
                border.width: 2;

                activeFocusOnTab: true;

                FileDialog {
                    id: fileDialog;
                    selectMultiple: true;
                    title: "Select games to import into your library"
                    onAccepted: { GameHasherController.scanForGames( fileUrls ); }
                }

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

