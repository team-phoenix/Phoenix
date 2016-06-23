import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.paths 1.0
import vg.phoenix.themes 1.0

import "../"
import "../Theme"

Item {
    Item {
        anchors {
            top: parent.top; bottom: parent.bottom; left: parent.left;
            /*topMargin: headerArea.height;*/ leftMargin: 30;
        }
        width: 620;
        property int verticalMargin: 10;

        // A handy clickable link that takes you straight to the core folder
        Rectangle {
            id: pathHelper;
            anchors { top: parent.top; horizontalCenter: parent.horizontalCenter; }
            anchors.topMargin: parent.verticalMargin;
            height: 30;
            width: parent.width;

            color: "transparent";
            border.color: activeFocus ? PhxTheme.common.menuItemHighlight : "transparent";
            border.width: 2;

            activeFocusOnTab: true;

            Image {
                anchors { right: pathHelperText.left; rightMargin: 8; }
                anchors.verticalCenter: pathHelper.verticalCenter;
                source: "../Assets/core.svg";
                smooth: false;
            }

            Text {
                id: pathHelperText;
                anchors.centerIn: parent;
                width: contentWidth;
                height: contentHeight;

                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
                text: "Core files must be placed in the core folder. Click here to open...";
                color: "white";
                font {
                    pixelSize: PhxTheme.common.baseFontSize;
                    family: PhxTheme.common.systemFontFamily;
                    bold: true;
                }
            }

            MouseArea {
                id: hintMouseArea;
                anchors.fill: parent;
                onClicked: {
                    Qt.openUrlExternally( "file:///" + PhxPaths.qmlCoreLocation() );
                }

                hoverEnabled: true;
                onEntered: { hintMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { hintMouseArea.cursorShape = Qt.ArrowCursor; }
            }
        }

        // A list of systems and the current core for that system
        // @disable-check M300
        PhxScrollView {
            id: scrollView;
            anchors { top: pathHelper.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; }
            anchors.topMargin: parent.verticalMargin;

            // @disable-check M300
            PhxListView {
                id: listView;
                anchors.fill: parent;
                anchors.rightMargin: 10;

                spacing: 3;
                model: CoreModel { }

                growOnFocusValue: 8;

                // Grab focus for ourselves on init
                Component.onCompleted: forceActiveFocus();

                header: Item {
                    anchors { left: parent.left; right: parent.right; }
                    height: 60;
                    Text {
                        id: headerText;
                        anchors.top: parent.top; anchors.left: parent.left;
                        width: parent.width;
                        height: 30;

                        verticalAlignment: Text.AlignVCenter;
                        horizontalAlignment: Text.AlignLeft;
                        text: "Core Settings";
                        color: "white";
                        font {
                            pixelSize: 14;
                            family: PhxTheme.common.systemFontFamily;
                            bold: true;
                        }
                    }

                    Text {
                        anchors.top: headerText.bottom; anchors.left: parent.left;
                        width: parent.width;
                        height: 20;

                        verticalAlignment: Text.AlignVCenter;
                        horizontalAlignment: Text.AlignLeft;
                        text: "Select the core (emulator) to be used for a particular system. If a core is unavailable, it will be marked in red. Click the button to restore the default core.";
                        wrapMode: Text.WordWrap;
                        color: PhxTheme.common.baseFontColor;
                        font {
                            pixelSize: 10;
                            family: PhxTheme.common.systemFontFamily;
                            bold: true;
                        }
                    }
                }

                delegate: FocusScope {
                    id: delegate;
                    anchors { left: parent.left; right: parent.right; }
                    height: 35;

                    Text {
                        anchors.left: parent.left;
                        anchors.leftMargin: 20;
                        anchors.verticalCenter: parent.verticalCenter;
                        Layout.fillWidth: true;
                        height: parent.height;
                        text: ( systemFriendlyName.localeCompare( "" ) ? systemFriendlyName : system ) + ":";
                        color: PhxTheme.common.highlighterFontColor;
                        verticalAlignment: Text.AlignVCenter;
                        font.family: PhxTheme.common.systemFontFamily;
                    }

                    // Cover only the left side with this
                    MouseArea {
                        anchors.left: parent.left;
                        anchors.right: row.right;
                        anchors.verticalCenter: parent.verticalCenter;
                        height: parent.height;
                        onClicked: {
                            listView.forceActiveFocus();
                            listView.currentIndex = index;
                        }
                    }

                    Row {
                        id: row;
                        anchors.right: parent.right;
                        anchors.rightMargin: 8;
                        height: parent.height;
                        spacing: 8;

                        Item {
                            anchors.verticalCenter: parent.verticalCenter;
                            width: 308;
                            height: parent.height;
                            focus: true;

                            Keys.onPressed: {
                                if( event.key === Qt.Key_Left ) {
                                    comboBox.currentIndex = Math.max( 0, comboBox.currentIndex - 1 );
                                    event.accepted = true;
                                } else if( event.key === Qt.Key_Right ) {
                                    comboBox.currentIndex = Math.min( cores.length - 1, comboBox.currentIndex + 1 );
                                    event.accepted = true;
                                } else if( event.key === Qt.Key_Up ) {
                                    listView.currentIndex = Math.max( 0, listView.currentIndex - 1 );
                                    event.accepted = true;
                                } else if( event.key === Qt.Key_Down ) {
                                    listView.currentIndex = Math.min( numSystems - 1, listView.currentIndex + 1 );
                                    event.accepted = true;
                                } else {
                                    event.accepted = false;
                                }
                            }

                            ComboBox {
                                id: comboBox;
                                model: cores;
                                anchors.verticalCenter: parent.verticalCenter;
                                width: 300;
                                height: parent.height;
                                activeFocusOnTab: false;

                                onPressedChanged: {
                                    if( pressed ) {
                                        listView.forceActiveFocus();
                                        listView.currentIndex = index;
                                    }
                                }

                                style: ComboBoxStyle {
                                    background: Rectangle {
                                        implicitHeight: control.height;
                                        implicitWidth: control.width;
                                        color: "#FFFFFF";
                                    }
                                    font: PhxTheme.common.systemFontFamily;
                                }

                                Component.onCompleted: {

                                    // Set the current core's index now
                                    currentIndex = currentCoreIndex;

                                    // Workaround, onCurrentIndexChanged is fired when the ComboBox is constructed.
                                    currentIndexChanged.connect( function() {
                                        listView.model.save( system, currentIndex );
                                    } );
                                }
                            }

                            Rectangle {
                                id: coreStatusIndicator;
                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: comboBox.right;
                                width: 8;
                                height: parent.height;
                                color: listView.model.coreExists( comboBox.currentText.replace( " (default)", "" ) ) ?
                                           PhxTheme.common.successColor : PhxTheme.common.errorColor;
                            }
                        }


                        Image {
                            id: resetCoreButton;
                            anchors.verticalCenter: parent.verticalCenter;
                            width: 20;
                            height: 20;
                            visible: comboBox.currentIndex !== defaultCoreIndex;
                            enabled: visible;
                            sourceSize { width: width; height: height; }
                            source: "../Assets/default.svg";
                            fillMode: Image.PreserveAspectFit;

                            MouseArea {
                                anchors.fill: parent;
                                onClicked: comboBox.currentIndex = defaultCoreIndex;
                            }
                        }

                        Item {
                            id: filler;
                            width: 20;
                            height: 20;
                            visible: !resetCoreButton.visible;
                            enabled: visible;
                        }
                    }
                }
            }
        }
    }
}
