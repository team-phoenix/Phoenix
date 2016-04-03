import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.scanner 1.0
import vg.phoenix.backend 1.0
import vg.phoenix.models 1.0
import vg.phoenix.paths 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: contentArea;
    color: PhxTheme.common.secondaryBackgroundColor;

    property alias contentLibraryModel: libraryModel;
    property alias contentStackView: contentAreaStackView;
    property alias contentLibrarySettingsView: librarySettingsView;
    property alias contentInputSettingsView: inputSettingsView;
    property alias contentSlider: zoomSlider;
    property alias contentBoxartGrid: boxartGridView;
    property alias boxartGrid: boxartGridView;

    property bool fullscreen: root.visibility === Window.FullScreen;
    property string screenIcon: {
        if( fullscreen ) screenIcon: "window.svg";
        else screenIcon: "fullscreen.svg";
    }

    Rectangle {
        id: headerArea;
        anchors { top: parent.top; left: parent.left; right: parent.right; }
        color: "transparent"
        height: 70;
        z: 1;

        Rectangle {
            anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: 30; }
            color: "#FFF";
            width: 250;
            radius: height / 2;
            height: 30;
            border.color: searchBar.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
            border.width: 2;

            // @disable-check M300
            PhxSearchBar {
                anchors { left: parent.left; leftMargin: 10; }
                id: searchBar;
                font.pixelSize: PhxTheme.common.baseFontSize;
                placeholderText: "";
                width: parent.width - 50;
                height: parent.height;
                textColor: "#333";

                Timer {
                    id: searchTimer;
                    interval: 300;
                    running: false;
                    repeat: false;
                    onTriggered: { libraryModel.setFilter( "title", "%" + searchBar.text + "%" ); }
                }
                onTextChanged: searchTimer.restart();
            }

            Image {
                anchors { right: parent.right; rightMargin: 10; verticalCenter: parent.verticalCenter; }
                width: 20;
                height: width;
                sourceSize { height: height; width: width; }
                source: searchBar.text === "" ? "search.svg" : "del.svg";
                MouseArea {
                    anchors.fill: parent;
                    onClicked: searchBar.text = "";
                }
            }
        }

        Row {
            anchors { verticalCenter: parent.verticalCenter; right: parent.right; rightMargin: 30; }
            spacing: 12;

            // Small rectangle
            Item {
                anchors { verticalCenter: parent.verticalCenter; }
                width: 12;
                height: parent.height;

                Rectangle {
                    anchors { verticalCenter: parent.verticalCenter; right: parent.right; }
                    border { width: 1; color: "#FFFFFF"; }
                    color: "transparent";
                    width: height;
                    height: 8;

                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: zoomSlider.value = Math.max( zoomSlider.minimumValue, zoomSlider.value - zoomSlider.stepSize );
                }
            }

            Slider {
                id: zoomSlider;
                anchors { verticalCenter: parent.verticalCenter; }
                width: 100;
                height: 30;
                minimumValue: 150;
                maximumValue: 450;
                value: 150;
                stepSize: 50;
                activeFocusOnPress: true;
                tickmarksEnabled: false;

                // FIXME: Animation looks awkward. Maybe instead of one rectangle centered vertically have two that
                // "grow" from the center (one facing up, one facing down)
                style: SliderStyle {
                    handle: Item {
                        width: 6;
                        height: zoomSlider.activeFocus ? 15 : 10;

                        Behavior on height { PropertyAnimation { duration: 250; } }

                        Rectangle {
                            id: handleRectangle;
                            anchors.fill: parent;
                            color: zoomSlider.activeFocus ? PhxTheme.common.menuItemHighlight : "#FFFFFF";

                            Behavior on color { ColorAnimation { duration: 250; } }
                        }
                    }

                    groove: Item {
                        width: control.width;
                        height: 1.5;

                        Rectangle {
                            anchors.fill: parent;
                            color: "#FFFFFF";
                            opacity: .35;
                        }
                    }
                }
            }

            // Large rectangle
            Item {
                anchors { verticalCenter: parent.verticalCenter; }
                width: 12;
                height: parent.height;

                Rectangle {
                    anchors { verticalCenter: parent.verticalCenter; }
                    border { width: 1; color: "#FFFFFF"; }
                    color: "transparent";
                    height: 12;
                    width: height;
                }

                MouseArea {
                    anchors.fill: parent;
                    onClicked: zoomSlider.value = Math.min( zoomSlider.maximumValue, zoomSlider.value + zoomSlider.stepSize );
                }
            }

            // Spacer
            Item { width: 12; height: 12; }

            // Fullscreen
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter;
                height: 32;
                width: height;
                color: fullscreenButton.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
                radius: width * 0.5;

                Image {
                    id: fullscreenButton;
                    anchors.centerIn: parent;
                    height: fullscreen ? 18 : 24;
                    width: height;

                    sourceSize { height: height; width: width; }
                    source: screenIcon;

                    activeFocusOnTab: true;

                    function toggleFullScreen() {
                        if( fullscreen ) root.visibility = Window.Windowed;
                        else root.visibility = Window.FullScreen;
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: fullscreenButton.toggleFullScreen();
                    }

                    Keys.onPressed: {
                        if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                            toggleFullScreen();
                            event.accepted = true;
                        }
                    }
                }
            }


            // Close
            Image {
                id: closeButton;
                anchors.verticalCenter: parent.verticalCenter;
                height: 14;
                width: 14;

                visible: root.visibility === Window.FullScreen;
                enabled: visible;

                activeFocusOnTab: enabled;

                sourceSize { height: height; width: width; }
                source: "close.svg";

                MouseArea {
                    anchors.fill: parent;
                    onClicked: Qt.quit();
                }

                Keys.onPressed: {
                    if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                        Qt.quit();
                        event.accepted = true;
                    }
                }

                Rectangle {
                    anchors.fill: parent;
                    color: "transparent";
                    border.color: closeButton.activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
                    border.width: 2;
                }
            }
        }
    }

    SqlThreadedModel {
        id: libraryModel;

        databaseSettings {
            connectionName: "USERDATA";
        }

        fileLocation: PhxPaths.qmlUserDataLocation() + '/' + "userdata.sqlite";

        autoCreate: true;

        tableName: "games";

        SqlColumn { name: "rowIndex"; type: "INTEGER PRIMARY KEY AUTOINCREMENT"; }
        SqlColumn { name: "title"; type: "TEXT NOT NULL"; }
        SqlColumn { name: "system"; type: "TEXT"; }
        SqlColumn { name: "region"; type: "TEXT"; }
        SqlColumn { name: "goodtoolsCode"; type: "TEXT"; }
        SqlColumn { name: "timePlayed"; type: "DATETIME"; }
        SqlColumn { name: "artworkUrl"; type: "TEXT"; }
        SqlColumn { name: "coreFilePath"; type: "TEXT"; }
        SqlColumn { name: "absolutePath"; type: "TEXT"; }
        SqlColumn { name: "absoluteFilePath"; type: "TEXT UNIQUE NOT NULL"; }
        SqlColumn { name: "crc32Checksum"; type: "TEXT"; }

        Component.onCompleted: {
            libraryModel.finishModelConstruction();

            /*
            var l = [];
            for ( var i=1; i < 500; i++ ) {
                l.push( { "absoluteFilePath": "/home/lee/shit" + i + ".sfc", "title": "SHIT" + i } );
            }

            libraryModel.addRows( l );
            */

            GameHasherController.scanCompleted.connect( libraryModel.addEntries );
        }
    }

    property bool currentlySuspended: typeof root.gameViewObject === 'undefined' ?
                                      false : root.gameViewObject.coreControl.state === Control.PAUSED;

    StackView {
        id: contentAreaStackView;
        initialItem: manualAddMode//boxartGridView;
        anchors.fill: parent;
        anchors.bottomMargin: currentlySuspended ? gameSuspendedArea.height : 0;

        property string currentObjectName: currentItem === null ? "" : currentItem.objectName;

        delegate: StackViewDelegate {
            function transitionFinished( properties ) { properties.exitItem.opacity = 1; }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem;
                    property: "opacity";
                    from: 0;
                    to: 1;
                }

                PropertyAnimation {
                    target: exitItem;
                    property: "opacity";
                    from: 1;
                    to: 0;
                }
            }
        }


        Item {
            id: manualAddMode;
            objectName: "ManualAddMode";
            visible: !contentAreaStackView.currentObjectName.localeCompare( objectName );

            SqlThreadedModel {
                id: tModel;
                fileLocation: PhxPaths.qmlMetadataLocation() + '/' + "libretro.sqlite";
                tableName: "system";
                databaseSettings {
                    connectionName: "MANUAL_ADD_MODE_LIBRETRO";
                }

                SqlColumn { name: "UUID"; type: "TEXT PRIMARY KEY"; }
                SqlColumn { name: "friendlyName"; type: "TEXT"; }
                SqlColumn { name: "shortName"; type: "TEXT"; }
                SqlColumn { name: "manufacturer"; type: "TEXT"; }

                Component.onCompleted: {
                    tModel.finishModelConstruction();
                    tModel.setFilter( "enabled", 1, SqlModel.Exact );
                    //tModel.setFilter( "")
                }
            }

            Rectangle {
                anchors.fill: parent;
                color: "transparent";

                ListView {
                    id: tableView;
                    anchors {
                        fill: parent;
                        margins: 100;
                    }

                    Rectangle {
                        anchors {
                            top: parent.top;
                            right: parent.right;
                        }

                        color: "red";
                        height: 25;
                        width: height;
                        radius: height / 2;

                        Text {
                            anchors.centerIn: parent;
                            text: "Close";
                            color: "white";
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                contentAreaStackView.push( { item: boxartGridView, replace: true } );
                            }
                        }
                    }

                    delegate: Rectangle {
                        width: parent.width * 0.8;
                        height: 50;
                        color: index % 2 == 0 ? "gray" : "white";

                        RowLayout {
                            anchors.fill: parent;
                            anchors.leftMargin: 12;
                            anchors.rightMargin: 12;

                            TextField {
                                id: titleField;
                                text: title;
                                implicitWidth: 300;
                                anchors {
                                    verticalCenter: parent.verticalCenter;
                                }


                            }

                            Rectangle {
                                anchors {
                                    top: parent.top;
                                    bottom: parent.bottom;
                                }

                                color: "orange";
                            }

                            ComboBox {
                                id: systemChoices;
                                anchors {
                                    verticalCenter: parent.verticalCenter;
                                }
                                textRole: "UUID";
                                implicitWidth: 250;
                                model: tModel;
                            }

                            Button {
                                anchors {
                                    verticalCenter: parent.verticalCenter;
                                }
                                text: "Add To Library";
                                onClicked: {
                                    libraryModel.addRow( { "title": titleField.text
                                                          , "system": systemChoices.currentText
                                                          , "region": region
                                                          , "artworkUrl": artworkUrl
                                                          , "absoluteFilePath": absoluteFilePath
                                                          , "crc32Checksum": crc32Checksum } );


                                    tableView.model.deleteRow( index, "absoluteFilePath", absoluteFilePath );

                                }
                            }
                        }
                    }


                    model: SqlThreadedModel {
                        id: manualAddModeModel;
                        tableName: "games";
                        autoCreate: true;
                        fileLocation: PhxPaths.qmlUserDataLocation() + "/manual_add_model.sqlite";

                        databaseSettings {
                            connectionName: "MANUAL_ADD_MODEL";
                        }

                        SqlColumn { name: "rowIndex"; type: "INTEGER PRIMARY KEY AUTOINCREMENT"; }
                        SqlColumn { name: "title"; type: "TEXT NOT NULL"; }
                        SqlColumn { name: "system"; type: "TEXT"; }
                        SqlColumn { name: "region"; type: "TEXT"; }
                        SqlColumn { name: "goodtoolsCode"; type: "TEXT"; }
                        SqlColumn { name: "timePlayed"; type: "DATETIME"; }
                        SqlColumn { name: "artworkUrl"; type: "TEXT"; }
                        SqlColumn { name: "coreFilePath"; type: "TEXT"; }
                        SqlColumn { name: "absolutePath"; type: "TEXT"; }
                        SqlColumn { name: "absoluteFilePath"; type: "TEXT UNIQUE NOT NULL"; }
                        SqlColumn { name: "crc32Checksum"; type: "TEXT"; }

                        Component.onCompleted: {
                            manualAddModeModel.finishModelConstruction();
                            GameHasherController.filesNeedAssignment.connect( function ( result ) {
                                manualAddModeModel.addEntries( result );
                                contentAreaStackView.push( { item: manualAddMode, replace: true }  );
                            });
                        }
                    }
                }
            }
        }


        BoxartGridView {
            id: boxartGridView;
            objectName: "BoxartGridView";
            visible: !contentAreaStackView.currentObjectName.localeCompare( objectName );
            enabled: visible;
        }

        DetailedGameView {
            id: detailGameView;
            objectName: "DetailedGameView";
            visible: !contentAreaStackView.currentObjectName.localeCompare( objectName );
            enabled: visible;
        }

        InputSettingsView {
            id: inputSettingsView;
            objectName: "InputSettingsView";
            visible: !contentAreaStackView.currentObjectName.localeCompare( objectName );
            enabled: visible;
        }

        LibrarySettingsView {
            id: librarySettingsView;
            objectName: "LibrarySettingsView";
            visible: !contentAreaStackView.currentObjectName.localeCompare( objectName );
            enabled: visible;
        }
    }


    GameSuspendedArea {
        id: gameSuspendedArea;
        objectName: "GameSuspendedArea";
        visible: currentlySuspended;
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }
        height: 65;
    }
}
