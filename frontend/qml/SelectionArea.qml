import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
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

        Row {
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                right: parent.right;
            }
        }

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

            Rectangle {
                property color defaultColor: "orange";
                color: defaultColor;
                Layout.fillWidth: true;
                height: 50;
                z: selectionAreaToolbar.z + 1;
                visible: root.gameViewObject.videoRender.coreState === Core.STATEPAUSED;

                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: 12;
                        verticalCenter: parent.verticalCenter;
                    }

                    elide: Text.ElideRight;
                    width: 100;

                    text: root.gameViewObject.coreGamePair[ "title" ];
                }

                MouseArea {
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onClicked: {
                        // Prevent user from clicking on anything while the transition occurs
                        root.disableMouseClicks();

                        // Destroy the compenent this MouseArea lives in
                        layoutStackView.pop();
                    }
                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
                    onEntered: {
                        parent.color = Qt.lighter( parent.defaultColor );
                        rootMouseArea.cursorShape = Qt.PointingHandCursor;
                    }
                    onExited: {
                        parent.color = parent.defaultColor;
                        rootMouseArea.cursorShape = Qt.ArrowCursor;
                    }
                }

                Row {

                    anchors {
                        top: parent.top;
                        bottom: parent.bottom;
                        right: parent.right;
                        topMargin: 3;
                        bottomMargin: 3;
                    }

                    Rectangle {
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                        }

                        width: 50;

                        radius: 6;
                        color: "blue";

                        Label {
                            anchors.centerIn: parent;
                            text: "Close";
                            width: parent.width;
                            elide: Text.ElideRight;
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                root.gameViewObject.videoRender.stop();
                            }
                        }

                    }
                }
            }

            // The buttons along the bottom that control the sectionsAreaStackView
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
                    ListElement { bgColor: "black"; label: "Add Games"; imageSource: "add.svg"; }
                    ListElement { bgColor: "black"; label: "Favorites"; imageSource: "collections.svg"; }
                    ListElement { bgColor: "black"; label: "Games"; imageSource: "games.png"; }
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


                delegate: Item {
                    height: parent.height;
                    width: selectionAreaToolbar.width / selectionAreaToolbar.count;

                    Row {
                        anchors {
                            right: parent.right;
                            bottom: parent.bottom;
                            bottomMargin: 6;
                        }
                    }

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
                                    if( contentArea.contentStackView.currentItem.objectName !== "BoxartGridView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentBoxartGrid, replace: true } );
                                    }
                                }
                                break;
                            case 2:
                                if ( sectionsAreaStackView.currentItem.objectName !== "PlatformsView" ) {
                                    sectionsAreaStackView.push( { item: platformsView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "BoxartGridView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentBoxartGrid, replace: true } );
                                    }
                                }
                                break;
                            case 3:
                                if ( sectionsAreaStackView.currentItem.objectName !== "SettingsView" ) {
                                    sectionsAreaStackView.push( { item: settingsView, replace: true } );
                                    if( contentArea.contentStackView.currentItem.objectName !== "InputView") {
                                        contentArea.contentStackView.push( { item: contentArea.contentInputView, replace: true } );
                                    }
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

