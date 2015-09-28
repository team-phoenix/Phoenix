import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.cache 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.backend 1.0

Rectangle {
    id: boxartGridBackground;
    DropdownMenu { id: dropDownMenu; }

    PhxScrollView {
        id: scrollView;
        anchors { fill: parent; topMargin: headerArea.height; }

        // The default of 20 just isn't fast enough
        __wheelAreaScrollSpeed: 100;

        /* Top drop shadow
        Rectangle {
            opacity: gridView.atYBeginning ? 0.0 : 0.3;
            z: 100;
            anchors { top: parent.top; left: parent.left; right: parent.right; }

            height: 25
            gradient: Gradient {
                GradientStop { position: 0.0; color: "black" }
                GradientStop { position: 1.0; color: "transparent" }
            }

            Behavior on opacity { PropertyAnimation { duration: 200 } }
        }

        // Bottom drop shadow
        Rectangle {
            opacity: gridView.atYEnd ? 0.0 : 0.3;
            z: 100;
            anchors { bottom: parent.bottom; left: parent.left; right: parent.right; }

            height: 25;
            gradient: Gradient {
                GradientStop { position: 0.0; color: "transparent"; }
                GradientStop { position: 1.0; color: "black"; }
            }

            Behavior on opacity { PropertyAnimation { duration: 200; } }
        }*/

        property int numItems: Math.floor( contentItem.width / contentArea.contentSlider.value );
        property int addToMarginsTotal: contentItem.width % contentArea.contentSlider.value;
        property int addToMargins: 0//addToMarginsTotal / numItems;
        onAddToMarginsChanged: {
            console.log( addToMargins );
        }

        contentItem: GridView {
            id: gridView;
            anchors {
                top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;
                leftMargin: searchBar.anchors.leftMargin; rightMargin: leftMargin;
            }

            // If the grid's width is less than the maxCellWidth, get
            // the grid to scale the size of the grid items, so that the transition looks really
            // seamless.
            cellHeight: contentArea.contentSlider.value;
            cellWidth: cellHeight;

            Behavior on cellHeight {
                NumberAnimation { duration: 200; }
            }

            model: libraryModel;

            // Define some transition animations
            property Transition transition: Transition { NumberAnimation { properties: "x,y"; duration: 250; } }
            property Transition transitionX: Transition { NumberAnimation { properties: "x"; duration: 250; } }

            // add: transition;
            // addDisplaced: transition;
            // displaced: transition;
            // move: transition;
            // moveDisplaced: transition;
            // populate: transitionX;
            // remove: transition;
            // removeDisplaced: transition;

            // Behavior on contentY { SmoothedAnimation { duration: 250; } }

            // Yes this isn't ideal, but it is a work around for the view resetting back to 0
            // whenever a game is imported.
            /*property real lastY: 0;

            onContentYChanged: {
                if (contentY == 0) {
                    console.log( contentY );
                    if ( Math.round( lastY ) !== 0.0 ) {
                        contentY = lastY;
                    }
                }
                else
                    lastY = contentY;
            }*/

            //clip: true
            boundsBehavior: Flickable.StopAtBounds;

            Component.onCompleted: { populate: transitionX; libraryModel.updateCount(); }

            property int itemMargin: 32;

            delegate: Rectangle {
                id: gridItem;
                width: gridView.cellWidth /*+ scrollView.addToMargins*/; height: gridView.cellHeight;
                color: "transparent";
                // border.color: "black";
                // border.width: 1;

                ColumnLayout {
                    spacing: 13;
                    anchors {
                        top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right;
                        topMargin: gridView.itemMargin/2; bottomMargin: gridView.itemMargin/2; leftMargin: gridView.itemMargin; rightMargin: gridView.itemMargin;
                    }

                    Rectangle {
                        id: gridItemImageContainer;
                        color: "transparent";
                        Layout.fillHeight: true;
                        Layout.fillWidth: true;

                        Image {
                            id: gridItemImage;
                            height: parent.height;
                            anchors { left: parent.left; right: parent.right; bottom: parent.bottom; }
                            visible: true;
                            asynchronous: true;

                            source: imageCacher.cachedUrl == "" ? "missingArtwork.png" : imageCacher.cachedUrl;
                            sourceSize { width: 400; height: 400; }
                            verticalAlignment: Image.AlignBottom;
                            fillMode: Image.PreserveAspectFit;

                            onStatusChanged: {
                                if ( status == Image.Error ) {
                                    console.log( "Error in " + source );
                                    gridItemImage.source = "missingArtwork.png";
                                }

                                // This is not triggered when source is an empty string
                                if ( status == Image.Null ) {
                                    console.log( "No image available for " + title );
                                }
                            }

                            Rectangle {
                                id: imageBackground;
                                anchors { top: parent.top; topMargin: 1; bottom: parent.bottom; bottomMargin: -border.width; left: parent.left; leftMargin: -border.width; right: parent.right; rightMargin: -border.width; }
                                z: gridItemImage.z - 1;
                                height: parent.paintedHeight;
                                width: parent.paintedWidth;
                                border.color: index === gridView.currentIndex ? PhxTheme.common.boxartSelectedBorderColor : PhxTheme.common.boxartNormalBorderColor;
                                border.width: 4;
                                color: "transparent";
                                radius: 2;
                            }

                            RectangularGlow {
                                anchors.fill: imageBackground;
                                glowRadius: 8;
                                spread: .15;
                                color: "#35000000";
                                cornerRadius: glowRadius;
                                z: imageBackground.z - 1;
                            }

                            MouseArea {
                                anchors.fill: parent;
                                onClicked: { gridView.currentIndex = index; }
                                onDoubleClicked: {

                                    // Prevent user from clicking on anything while the transition occurs
                                    root.disableMouseClicks();

                                    // Don't check the mouse until the transition's done
                                    rootMouseArea.hoverEnabled = false;

                                    // Let the user know we're thinking!
                                    rootMouseArea.cursorShape = Qt.BusyCursor;

                                    // Do the assignment that triggers the game launch
                                    root.gameViewObject.coreGamePair = { "corePath": coreFilePath
                                                                       , "gamePath": absoluteFilePath
                                                                       , "title": title };

                                    layoutStackView.pop();
                                }
                            }

                            ImageCacher {
                                id: imageCacher;
                                imageUrl: artworkUrl;
                                identifier: sha1;
                                Component.onCompleted: cache();
                            }

                            // ToolTip Title
                            // ToolTipArea { text: title; tip {  x: 0; y: parent.width + 24; } }

                            // Decorations
                            Rectangle {
                                id: imageTopAccent;
                                anchors.horizontalCenter: gridItemImage.horizontalCenter;
                                y: gridItemImage.y + ( gridItemImage.height - gridItemImage.paintedHeight );
                                width: gridItemImage.paintedWidth;
                                height: 1;
                                opacity: 0.35;
                                color: "white";
                            }

                        }
                    }

                    // Games titles
                    Label {
                        id: titleText;
                        text: title;
                        color: PhxTheme.common.highlighterFontColor;
                        Layout.fillWidth: true;
                        elide: Text.ElideRight;
                        font { pixelSize: 12; bold: true; }
                    }

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

