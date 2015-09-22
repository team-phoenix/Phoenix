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

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 2;
                color: "black";
                opacity: 0.2;
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "white";
                opacity: 0.05;
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
                color: "yellow";
                Layout.fillWidth: true;
                height: 50;
                z: selectionAreaToolbar.z + 1;
                visible: root.gameViewObject.videoRender.coreState === Core.STATEPAUSED;

                Label {
                    anchors.centerIn: parent;
                    color: "white";
                    text: root.gameViewObject.loadedGame;
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
                        color: "red";

                        Label {
                            anchors.centerIn: parent;
                            text: "Play";
                            width: parent.width;
                            elide: Text.ElideRight;
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                layoutStackView.pop();
                            }
                        }
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
                                console.log( "Make it close the game" );
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

                    Rectangle {
                        anchors {
                            top: parent.top;
                            right: parent.right;
                            left: parent.left;
                            rightMargin: 2;
                        }
                        height: 3;
                        color: "black";
                        opacity: 0.2;

                    }




                delegate: Item {
                    height: parent.height;
                    width: selectionAreaToolbar.width / selectionAreaToolbar.count;

                    Rectangle {
                        id: topAccent;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top: parent.top;
                            rightMargin: 4;
                            topMargin: 3;
                        }
                        height: 1;
                        color: "white";
                        opacity: 0.05;
                    }

                    Row {
                        anchors {
                            right: parent.right;
                            top: topAccent.bottom;
                            bottom: parent.bottom;
                            bottomMargin: 6;
                        }

                        Rectangle {
                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                                topMargin: 0;
                            }
                            width: 1;
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "white"; }
                                GradientStop { position: 0.7; color: "white"; }
                                GradientStop { position: 1.0; color: "transparent"; }
                            }

                            opacity: 0.03;
                        }

                        Rectangle {
                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                                topMargin: -1;
                            }
                            width: 3;
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "black"; }
                                GradientStop { position: 0.7; color: "black"; }
                                GradientStop { position: 1.0; color: "transparent"; }
                            }

                            opacity: 0.2;
                        }

                        Rectangle {

                            anchors {
                                top: parent.top;
                                bottom: parent.bottom;
                                topMargin: -1;
                            }
                            width: 1;
                            gradient: Gradient {
                                GradientStop { position: 0.0; color: "white"; }
                                GradientStop { position: 0.7; color: "white"; }
                                GradientStop { position: 1.0; color: "transparent"; }
                            }

                            opacity: 0.03;
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

