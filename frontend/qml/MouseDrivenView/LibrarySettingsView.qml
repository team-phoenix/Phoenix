import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.paths 1.0
import vg.phoenix.themes 1.0

Item {
    id: librarySettingsView;

    Item {
        anchors {
            top: parent.top; bottom: parent.bottom; left: parent.left;
            topMargin: headerArea.height; leftMargin: 30;
        }
        width: 600;
        property int verticalMargin: 10;

        Item {
            id: pathHelper;
            anchors { top: parent.top; horizontalCenter: parent.horizontalCenter; }
            anchors.topMargin: parent.verticalMargin;
            height: 30;
            width: parent.width;

            Image {
                anchors { right: pathHelperText.left; rightMargin: 8; }
                anchors.verticalCenter: pathHelper.verticalCenter;
                source: "settings.svg";
            }

            Text {
                id: pathHelperText;
                anchors.centerIn: parent;
                width: contentWidth;
                height: contentHeight;

                verticalAlignment: Text.AlignVCenter;
                horizontalAlignment: Text.AlignHCenter;
                text: "Cores files must be placed in the core folder. Click here to open...";
                color: "white";
                font {
                    pixelSize: PhxTheme.common.baseFontSize;
                    family: PhxTheme.common.systemFontFamily;
                    bold: true;
                }
            }

            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    Qt.openUrlExternally( PhxPaths.qmlCoreLocation() );
                }

                hoverEnabled: true;
                onEntered: { rootMouseArea.cursorShape = Qt.PointingHandCursor; }
                onExited: { rootMouseArea.cursorShape = Qt.ArrowCursor; }
                Component.onCompleted: console.log( width + " " + height );
            }
        }

        PhxScrollView {
            id: scrollView;
            anchors { top: pathHelper.bottom; left: parent.left; right: parent.right; bottom: parent.bottom; }
            anchors.topMargin: parent.verticalMargin;

            // The default of 20 just isn't fast enough
            __wheelAreaScrollSpeed: 100;

            ListView {
                id: listView;
                anchors.fill: parent;
                anchors.rightMargin: 10;
                spacing: 3;
                model: CoreModel { }

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

                delegate: Item {
                    anchors { left: parent.left; right: parent.right; }
                    height: 35;

                    Text {
                        anchors.left: parent.left;
                        anchors.verticalCenter: parent.verticalCenter;
                        Layout.fillWidth: true;
                        height: parent.height;
                        text: system + ":";
                        color: PhxTheme.common.highlighterFontColor;
                        verticalAlignment: Text.AlignVCenter;
                        font.family: PhxTheme.common.systemFontFamily;
                    }

                    Row {
                        anchors.right: parent.right;
                        anchors.rightMargin: 8;
                        height: parent.height;
                        spacing: 8;

                        Item {
                            anchors.verticalCenter: parent.verticalCenter;
                            width: 308;
                            height: parent.height;

                            ComboBox {
                                id: comboBox;
                                model: cores;
                                anchors.verticalCenter: parent.verticalCenter;
                                width: 300;
                                height: parent.height;

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
                                    });
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
                            width: 15;
                            height: 15;
                            visible: comboBox.currentIndex !== defaultCoreIndex;
                            enabled: visible;
                            sourceSize { width: width; height: height; }
                            source: "close.svg";
                            fillMode: Image.PreserveAspectFit;
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: comboBox.currentIndex = defaultCoreIndex;
                            }
                        }
                        Item {
                            id: filler;
                            width: 15;
                            height: 15;
                            visible: !resetCoreButton.visible;
                            enabled: visible;
                        }
                    }
                }
            }
        }
    }
}
