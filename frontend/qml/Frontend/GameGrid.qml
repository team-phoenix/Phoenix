import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.Phoenix.Cache 1.0
import Phoenix.Backend 1.0
import Phoenix.Launcher 1.0
import Phoenix.Models 1.0
import Phoenix.Paths 1.0

import Phoenix.Emulator 1.0
import Phoenix.Settings 1.0
import Phoenix.Theme 1.0
import Phoenix.Util 1.0

FocusScope {
    PhxScrollView {
        id: scrollView;
        anchors.fill: parent;

        // How much do you add to each grid entry's width so that the sum of a row's widths equal the GridView's width?
        property double addToMargins: 0;

        // Don't set the actual binding until we're fully initalized. You'll get binding loop warnings otherwise
        Component.onCompleted: {
            addToMargins = Qt.binding( function() {
                return contentItem.width % zoomSlider.value
                        / Math.floor( contentItem.width / zoomSlider.value );
            } );
        }

        PhxGridView {
            id: gameGrid;
            anchors {
                fill: parent;
                leftMargin: searchBar.anchors.leftMargin; rightMargin: leftMargin;
            }

            Component.onCompleted: {
                // Try to get the library to give us an accurate picture of how tall the GridView really is
                //libraryModel.updateCount();

                // Sort in order
                //libraryModel.sort( 1, Qt.AscendingOrder );

                // Grab keyboard focus, this is the object that we decided gets focus initially
                forceActiveFocus();

                model = libraryModel;

                if( libraryModel.rowCount() > 0 ) currentIndex = 0;
            }

            //model: libraryModel;

            // If the grid's width is less than the maxCellWidth, get
            // the grid to scale the size of the grid items, so that the transition looks really
            // seamless.
            cellWidth: cellHeight + scrollView.addToMargins;
            cellHeight: zoomSlider.value;

            // Smoothly animate the cells getting resized by the slider
            //Behavior on cellHeight { NumberAnimation { duration: 200; } }
            //Behavior on cellWidth { NumberAnimation { duration: 200; } } // FIXME: This looks too awkward to keep (try moving the slider)

            // Minimum amount of spacing between grid entries
            property int itemMargin: 16;

            // FIXME: Initial transition, when set, results in all the items being cramped into the top left corner
            // Is the model working properly?

            // Define some transition animations
            //property Transition transition: Transition { NumberAnimation { properties: "x,y"; duration: 250; } }
            //property Transition transitionX: Transition { NumberAnimation { properties: "x"; duration: 250; } }

            //add: transition;
            //addDisplaced: transition;
            //displaced: transition;
            //move: transition;
            //moveDisplaced: transition;
            //populate: transitionX;
            //remove: transition;
            //removeDisplaced: transition;

            // Behavior on contentY { SmoothedAnimation { duration: 250; } }

            // A workaround for the view resetting back to 0 whenever a game is imported
//            property real lastY: 0;

//            onContentYChanged: {
//                if (contentY == 0) {
//                    if ( Math.round( lastY ) !== 0.0 ) {
//                        contentY = lastY;
//                    }
//                }
//                else
//                    lastY = contentY;
//            }

            delegate: Item {
                id: gridItem;
                width: gameGrid.cellWidth - scrollView.addToMargins; height: gameGrid.cellHeight;
                anchors.rightMargin: scrollView.addToMargins;

                Keys.onPressed: {
                    if( event.key === Qt.Key_Enter || event.key === Qt.Key_Return ) {
                        launchGame();
                    }
                }

                function launchGame() {
                    var core = coreFilePath;
                    if ( core === "" ) {
                        core = gameLauncher.getDefaultCore( system );
                    }

                    var game = gameLauncher.trimmedGame( absoluteFilePath );

                    if ( gameLauncher.verify( core, game ) ) {
                        // Set up the packet of information to pass to GameConsole
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
                        gameConsole.source = dict;

                        // Begin loading

                        // TODO: Ask user if they want to do this?
                        if( phoenix.state === "Minimized" ) {
                            phoenix.autoLoadFlag = true;
                            phoenix.state = "SilentlyUnloading";
                        }

                        else
                            phoenix.state = "Loading";
                    }
                }

                MouseArea {
                    id: gridItemMouseArea;
                    anchors.fill: parent;
                    hoverEnabled: true;
                    onClicked: { gameGrid.currentIndex = index; gameGrid.forceActiveFocus(); }
                    onDoubleClicked: launchGame();
                }

                ColumnLayout {
                    spacing: 13;
                    anchors {
                        fill: parent;
                        topMargin: gameGrid.itemMargin; bottomMargin: gameGrid.itemMargin;
                        leftMargin: gameGrid.itemMargin; rightMargin: gameGrid.itemMargin;
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
                            source: imageCacher.cachedUrl == "" ? "../Assets/noartwork.png" : imageCacher.cachedUrl;
                            sourceSize { height: zoomSlider.maximumValue; width: zoomSlider.maximumValue; }
                            verticalAlignment: Image.AlignBottom;
                            fillMode: Image.PreserveAspectFit;

                            onStatusChanged: {
                                if ( status == Image.Error ) {
                                    console.log( "Error in " + source );
                                    gridItemImage.source = "../Assets/noartwork.png";
                                }

                                // This is not triggered when source is an empty string
                                if ( status == Image.Null ) {
                                    console.log( "No image available for " + title );
                                }
                            }

                            BusyIndicator {
                                anchors.fill: parent;
                                anchors.margins: 8;
                                running: parent.status === Image.Loading;
                            }

                            // BoxArt: Glass line effect
                            Rectangle {
                                anchors { bottom: parent.bottom; bottomMargin: parent.paintedHeight - 1; horizontalCenter: parent.horizontalCenter; }
                                width: parent.paintedWidth ;
                                height: 1;
                                color: Qt.rgba( 255,255,255,.25 );
                            }

                            // BoxArt: Outer Border
                            Rectangle {
                                anchors { bottom: parent.bottom; topMargin: -border.width; bottomMargin: -border.width; leftMargin: -border.width; rightMargin: -border.width; horizontalCenter: parent.horizontalCenter; }
                                id: imageBackground;
                                z: gridItemImage.z - 1;
                                height: parent.paintedHeight + border.width * 2;
                                width: parent.paintedWidth + border.width * 2;
                                border.color: index === gameGrid.currentIndex && gameGrid.activeFocus ? PhxTheme.common.boxartSelectedBorderColor : PhxTheme.common.boxartNormalBorderColor;
                                border.width: 2 + ( zoomSlider.value / 50 );
                                color: "transparent";
                                radius: 3;
                            }

                            ImageCacher {
                                id: imageCacher;
                                imageUrl: artworkUrl;
                                identifier: crc32Checksum;
                                Component.onCompleted: cache();
                            }
                        }
                    }

                    // A label for the game's title
                    MarqueeText {
                        id: titleText;
                        Layout.fillWidth: true;
                        height: 20;
                        text: title;
                        spacing: zoomSlider.value / 10;
                        color: PhxTheme.common.highlighterFontColor;
                        fontSize: PhxTheme.common.baseFontSize;
                        running: index === gameGrid.currentIndex && gameGrid.activeFocus || gridItemMouseArea.containsMouse;
                        pixelsPerFrame: zoomSlider.value / 100;

                        Connections {
                            target: gameGrid;
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
