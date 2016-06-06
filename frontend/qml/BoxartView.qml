import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.cache 1.0
import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.paths 1.0
import vg.phoenix.models 1.0

import "../Util"

FocusScope {
    id: boxartGridBackground;

    // @disable-check M300
    //DropdownMenu { id: dropDownMenu; }

    // @disable-check M300
    ScrollView {
        id: scrollView;
        anchors { fill: parent; topMargin: 25; }

        // How much do you add to each grid entry's width so that the sum of a row's widths equal the GridView's width?
        property double addToMargins: 0;


        // @disable-check M300
        GridView {
            id: gridView;
            anchors {
                top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;
                leftMargin: 25; rightMargin: leftMargin;
            }

            Component.onCompleted: {
                // Try to get the library to give us an accurate picture of how tall the GridView really is
                //libraryModel.updateCount();

                // Sort in order
                //libraryModel.sort( 1, Qt.AscendingOrder );

                // Grab keyboard focus, this is the object that we decided gets focus initially
                forceActiveFocus();
            }

            model: SqlThreadedModel {
                id: libraryModel;

                databaseSettings {
                    connectionName: "USERDATA";
                }

                fileLocation: PhxPaths.qmlUserDataLocation() + "/userdata.sqlite";

                autoCreate: true;
                //selectStatement: "SELECT * FROM games ORDER BY title DESC";

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
                    libraryModel.setOrderBy( "title", SqlModel.ASC );
                    libraryModel.finishModelConstruction();
                    GameHasherController.scanCompleted.connect( libraryModel.addEntries );
                    //libraryModel.attachDatabase( "/home/path/to/database.sqlite", "an_alias" );
                }
            }

            // If the grid's width is less than the maxCellWidth, get
            // the grid to scale the size of the grid items, so that the transition looks really
            // seamless.
            cellWidth: 400;
            cellHeight: 400;
            //cellHeight: contentArea.contentSlider.value;

            // Smoothly animate the cells getting resized by the slider
            Behavior on cellHeight { NumberAnimation { duration: 200; } }
            // Behavior on cellWidth { NumberAnimation { duration: 200; } } // FIXME: This looks too awkward to keep (try moving the slider)

            // Minimum amount of spacing between grid entries
            property int itemMargin: 16;

            // FIXME: Initial transition, when set, results in all the items being cramped into the top left corner
            // Is the model working properly?

            // Define some transition animations
            // property Transition transition: Transition { NumberAnimation { properties: "x,y"; duration: 250; } }
            // property Transition transitionX: Transition { NumberAnimation { properties: "x"; duration: 250; } }

            // add: transition;
            // addDisplaced: transition;
            // displaced: transition;
            // move: transition;
            // moveDisplaced: transition;
            // populate: transitionX;
            // remove: transition;
            // removeDisplaced: transition;

            // Behavior on contentY { SmoothedAnimation { duration: 250; } }

            // A work around for the view resetting back to 0 whenever a game is imported
            property real lastY: 0;

            onContentYChanged: {
                if (contentY == 0) {
                    // console.log( contentY );
                    if ( Math.round( lastY ) !== 0.0 ) {
                        contentY = lastY;
                    }
                }
                else
                    lastY = contentY;
            }

            delegate: Item {
                id: gridItem;
                width: gridView.cellWidth - scrollView.addToMargins; height: gridView.cellHeight;
                anchors.rightMargin: scrollView.addToMargins;

                Keys.onPressed: {
                    if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                        launchGame();
                    }
                }

                function launchGame() {
                    var core = coreFilePath;
                    if ( core === "" ) {
                        console.log( "Core must be set")
                        //core = gameLauncher.getDefaultCore( system )
                    }

                    //var game = gameLauncher.trimmedGame( absoluteFilePath );

                    //if ( gameLauncher.verify( core, game ) ) {

                        // Prevent user from clicking on anything while the transition occurs
                        //root.disableMouseClicks();

                        // Don't check the mouse until the transition's done
                        //rootMouseArea.hoverEnabled = false;

                        // Let the user know we're thinking!
                        //rootMouseArea.cursorShape = Qt.WaitCursor;

                        // Set window title to game title
                        //root.title = "Loading - " + title;
                        console.log( title );
                        //console.log( root.title );

                        // Set up the packet of information to pass to CoreControl
                        var dict = {};
                        dict[ "type" ] = "libretro";
                        dict[ "core" ] = core;
                        dict[ "game" ] = game;
                        dict[ "systemPath" ] = PhxPaths.qmlFirmwareLocation();
                        dict[ "savePath" ] = PhxPaths.qmlSaveLocation();

                        // Extra stuff
                        dict[ "title" ] = title;
                        dict[ "system" ] = system;
                        dict[ "artworkURL" ] = imageCacher.cachedUrl;

                        // Assign the source
                        //root.gameViewObject.coreControl.source = dict;

                        // Connect the next callback in the chain to be called once the load begins/ends
                        //root.gameViewObject.controlOutput.stateChanged.connect( root.stateChangedCallback );

                        // Begin the load
                        // Execution will continue in stateChangedCallback() once CoreControl changes state
                        //root.gameViewObject.coreControl.load();
                    //}
                }

                MouseArea {
                    id: gridItemMouseArea;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onClicked: { gridView.currentIndex = index; gridView.forceActiveFocus(); }
                    onDoubleClicked: launchGame();
                }

                ColumnLayout {
                    spacing: 13;
                    anchors {
                        top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;
                        topMargin: gridView.itemMargin; bottomMargin: gridView.itemMargin;
                        leftMargin: gridView.itemMargin; rightMargin: gridView.itemMargin;
                    }

                    property alias gridItemMouseArea: gridItemMouseArea;

                    Rectangle {
                        id: gridItemImageContainer;
                        color: "transparent";
                        Layout.fillHeight: true;
                        Layout.fillWidth: true;

                        Image {
                            id: gridItemImage;
                            height: parent.height;
                            anchors { top: parent.top; left: parent.left; right: parent.right; bottom: parent.bottom; leftMargin: -1; }
                            visible: true;
                            asynchronous: true;
                            source: imageCacher.cachedUrl == "" ? "noartwork.png" : imageCacher.cachedUrl;
                            sourceSize { height: 300; width: 300; }
                            verticalAlignment: Image.AlignBottom;
                            fillMode: Image.PreserveAspectFit;

                            onStatusChanged: {
                                if ( status == Image.Error ) {
                                    console.log( "Error in " + source );
                                    gridItemImage.source = "noartwork.png";
                                }

                                // This is not triggered when source is an empty string
                                if ( status == Image.Null ) {
                                    console.log( "No image available for " + title );
                                }
                            }

                            // BoxArt: Glass line effect
                            Rectangle {
                                anchors { bottom: parent.bottom; bottomMargin: parent.paintedHeight - 1; horizontalCenter: parent.horizontalCenter; }
                                width: parent.paintedWidth ;
                                height: 1;
                                color: Qt.rgba(255,255,255,.25);
                            }

                            /* BoxArt: Inner Border
                            Rectangle {
                                anchors { bottom: parent.bottom; topMargin: -border.width; bottomMargin: -border.width; leftMargin: -border.width; rightMargin: -border.width; horizontalCenter: parent.horizontalCenter; }
                                z: gridItemImage.z + 1;
                                height: parent.paintedHeight + border.width;
                                width: parent.paintedWidth + border.width;
                                border.color: Qt.rgba(0,0,0,.75);
                                border.width: 1;
                                color: "transparent";
                            } */

                            // BoxArt: Outer Border
                            Rectangle {
                                anchors { bottom: parent.bottom; topMargin: -border.width; bottomMargin: -border.width; leftMargin: -border.width; rightMargin: -border.width; horizontalCenter: parent.horizontalCenter; }
                                id: imageBackground;
                                z: gridItemImage.z - 1;
                                height: parent.paintedHeight + border.width * 2;
                                width: parent.paintedWidth + border.width * 2;
                                border.color: index === gridView.currentIndex && gridView.activeFocus ? PhxTheme.common.boxartSelectedBorderColor : PhxTheme.common.boxartNormalBorderColor;
                                border.width: 2 + ( contentArea.contentSlider.value / 50 );
                                color: "transparent";
                                radius: 3;
                            }

                            /* BoxArt Shadow
                            RectangularGlow {
                                anchors.bottom: parent.bottom;
                                anchors.horizontalCenter: parent.horizontalCenter;
                                height: parent.paintedHeight;
                                width: parent.paintedWidth;
                                glowRadius: 1+(contentArea.contentSlider.value/50);
                                spread: .1;
                                color: "#50000000";
                                cornerRadius: glowRadius;
                                z: imageBackground.z - 1;
                            } */

                            ImageCacher {
                                id: imageCacher;
                                imageUrl: artworkUrl;
                                identifier: crc32Checksum;
                                Component.onCompleted: cache();
                            }

                            // ToolTip Title
                            // ToolTipArea { text: title; tip {  x: 0; y: parent.width + 24; } }

                        }

                    }

                    // A label for the game's title
                    MarqueeText {
                        id: titleText;
                        Layout.fillWidth: true;
                        height: 20;
                        text: title;
                        spacing: contentArea.contentSlider.value / 10;
                        color: PhxTheme.common.highlighterFontColor;
                        fontSize: PhxTheme.common.baseFontSize;
                        running: index === gridView.currentIndex && gridView.activeFocus || gridItemMouseArea.containsMouse;
                        pixelsPerFrame: contentArea.contentSlider.value / 100;

                        Connections {
                            target: gridView;
                            onCellWidthChanged: titleText.handleSituationChanged();
                        }
                    }

                    // For debugging the above MarqueeText
                    // Text {
                    //     text: titleText.state;
                    //     anchors.top: titleText.bottom;
                    //     color: "red";
                    //     Layout.fillWidth: true;
                    //     horizontalAlignment: Text.AlignHCenter;
                    //     elide: Text.ElideRight;
                    // }

                    /*Text {
                        id: platformText;
                        anchors {
                            top: titleText.bottom;
                            topMargin: 0;
                        }

                        text: system;
                        color: PhxTheme.common.baseFontColor;
                        Layout.fillWidth: true;
                        elide: Text.ElideRight;
                    }
                    Text {
                        id: absPath;
                        anchors {
                            top: platformText.bottom;

                        }
                        text: sha1; // absolutePath
                        color: PhxTheme.common.baseFontColor;
                        Layout.fillWidth: true;
                        elide: Text.ElideMiddle;
                    }*/
                }
            }
        }
    }
}
