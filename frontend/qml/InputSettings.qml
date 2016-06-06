import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0

import vg.phoenix.models 1.0
import vg.phoenix.paths 1.0
import vg.phoenix.themes 1.0

import "../"

Rectangle {
    color: PhxTheme.common.secondaryBackgroundColor;

    // Make the roles of listView's model available to the entire Component
    property PhxListView listView: scrollView.listView;
    property Item currentItem: listView.currentItem;
    property string currentGUID: currentItem == null ? "" : currentItem.guidProxy;
    property var remapData: currentItem == null ? [] : currentItem.remapDataProxy;
    property bool available: currentItem == null ? false : currentItem.availableProxy;
    property bool pressed: currentItem == null ? false : currentItem.pressedProxy;
    property var button: currentItem == null ? [] : currentItem.buttonProxy;
    property var hat: currentItem == null ? [] : currentItem.hatProxy;
    property var axis: currentItem == null ? [] : currentItem.axisProxy;
    property int numButtons: currentItem == null ? 0 : currentItem.numButtonsProxy;
    property int numHats: currentItem == null ? 0 : currentItem.numHatsProxy;
    property int numAxes: currentItem == null ? 0 : currentItem.numAxesProxy;
    property var deadzone: currentItem == null ? [] : currentItem.deadzoneProxy;
    property var deadzoneMode: currentItem == null ? [] : currentItem.deadzoneModeProxy;

    ColumnLayout {
        anchors.fill: parent;
        spacing: 0;
        Item {
            Layout.fillWidth: true;
            Layout.preferredHeight: 200;
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft;
            RowLayout {
                id: row;
                anchors { top: parent.top; bottom: parent.bottom; }
                spacing: 10;

                // A list of available GUIDs to remap
                PhxScrollView {
                    id: scrollView;

                    Layout.fillHeight: true;
                    Layout.fillWidth: true;
                    Layout.preferredWidth: 400;

                    // Make listView available from Row
                    property PhxListView listView: listView;
                    PhxListView {
                        id: listView;
                        anchors.fill: parent;

                        orientation: ListView.Vertical;

                        model: remapperModel;

                        Component.onCompleted: forceActiveFocus();

                        delegate: Item {
                            height: PhxTheme.common.menuItemHeight;
                            anchors { left: parent.left; right: parent.right; }

                            // Make these roles available outside the delegate Component
                            property string guidProxy: GUID;
                            property var remapDataProxy: remapData;
                            property bool availableProxy: available;
                            property bool pressedProxy: pressed;
                            property var buttonProxy: button;
                            property var hatProxy: hat;
                            property var axisProxy: axis;
                            property int numButtonsProxy: numButtons;
                            property int numHatsProxy: numHats;
                            property int numAxesProxy: numAxes;
                            property var deadzoneProxy: deadzone;
                            property var deadzoneModeProxy: deadzoneMode;

                            MarqueeText {
                                id: platformText;
                                anchors { verticalCenter: parent.verticalCenter; left: parent.left; right: parent.right; leftMargin: PhxTheme.common.menuItemMargin; rightMargin: PhxTheme.common.menuItemMargin; }
                                horizontalAlignment: Text.AlignLeft;
                                text: friendlyName + ( GUID !== "" ? ( " (" + GUID + ( available ? ")" : ")(disconnected)" ) ) : "" );
                                fontSize: PhxTheme.common.baseFontSize + 1;
                                color: available ? PhxTheme.common.menuSelectedColor : PhxTheme.selectionArea.baseFontColor;
                                spacing: 40;
                                running: listView.currentIndex === index || mouseArea.containsMouse;
                                pixelsPerFrame: 2.0;
                                bold: index === listView.currentIndex;
                            }

                            Rectangle {
                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: platformText.right;
                                width: 8;
                                height: PhxTheme.common.menuItemHeight - 5;
                                color: pressed ? PhxTheme.common.successColor : PhxTheme.common.errorColor;
                            }

                            MouseArea {
                                id: mouseArea;
                                anchors.fill: parent;
                                hoverEnabled: true;
                                onClicked: if( listView.currentIndex !== index && !remapperModel.remapMode ) listView.currentIndex = index;
                            }
                        }
                    }
                }

                // Remapping data
                GridLayout {
                    id: grid;

                    Layout.alignment: Qt.AlignLeft | Qt.AlignTop;
                    Layout.minimumWidth: 600;

                    columns: 4;
                    rowSpacing: 10;
                    columnSpacing: 10;

                    property color textColor: available ? "white" : "gray";

                    // Face buttons
                    Text {
                        color: grid.textColor;
                        text: "A: " + remapData[ "a" ];
                        Layout.column: 0; Layout.row: 0;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "a" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "B: " + remapData[ "b" ];
                        Layout.column: 0; Layout.row: 1;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "b" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "X: " + remapData[ "x" ];
                        Layout.column: 0; Layout.row: 2;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "x" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Y: " + remapData[ "y" ];
                        Layout.column: 0; Layout.row: 3;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "y" ); }
                    }

                    // D-Pad
                    Text {
                        color: grid.textColor;
                        text: "Up: " + remapData[ "dpup" ];
                        Layout.column: 1; Layout.row: 0;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "dpup" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Down: " + remapData[ "dpdown" ];
                        Layout.column: 1; Layout.row: 1;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "dpdown" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Left: " + remapData[ "dpleft" ];
                        Layout.column: 1; Layout.row: 2;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "dpleft" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Right: " + remapData[ "dpright" ];
                        Layout.column: 1; Layout.row: 3;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "dpright" ); }
                    }

                    // Center buttons
                    Text {
                        color: grid.textColor;
                        text: "Back: " + remapData[ "back" ];
                        Layout.column: 2; Layout.row: 0;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "back" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Guide: " + remapData[ "guide" ];
                        Layout.column: 2; Layout.row: 1;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "guide" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Start: " + remapData[ "start" ];
                        Layout.column: 2; Layout.row: 2;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "start" ); }
                    }

                    // Shoulder buttons
                    Text {
                        color: grid.textColor;
                        text: "L: " + remapData[ "leftshoulder" ];
                        Layout.column: 3; Layout.row: 0;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "leftshoulder" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "R: " + remapData[ "rightshoulder" ];
                        Layout.column: 3; Layout.row: 1;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "rightshoulder" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "L2: " + remapData[ "lefttrigger" ];
                        Layout.column: 3; Layout.row: 2;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "lefttrigger" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "R2: " + remapData[ "righttrigger" ];
                        Layout.column: 3; Layout.row: 3;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "righttrigger" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "L3: " + remapData[ "leftstick" ];
                        Layout.column: 3; Layout.row: 4;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "leftstick" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "R3: " + remapData[ "rightstick" ];
                        Layout.column: 3; Layout.row: 5;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "rightstick" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Left analog X: " + remapData[ "leftx" ];
                        Layout.column: 4; Layout.row: 0;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "leftx" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Left analog Y: " + remapData[ "lefty" ];
                        Layout.column: 4; Layout.row: 1;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "lefty" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Right analog X: " + remapData[ "rightx" ];
                        Layout.column: 4; Layout.row: 2;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "rightx" ); }
                    }
                    Text {
                        color: grid.textColor;
                        text: "Right analog Y: " + remapData[ "righty" ];
                        Layout.column: 4; Layout.row: 3;
                        MouseArea { anchors.fill: parent; onClicked: if( available ) remapperModel.beginRemapping( currentGUID, "righty" ); }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 120;
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft;
            color: PhxTheme.common.primaryBackgroundColor;
            ColumnLayout {
                anchors.fill: parent;
                Label {
                    Layout.preferredWidth: 120;
                    Layout.preferredHeight: 30;
                    text: "Raw SDL Data:";
                    color: "white";
                    horizontalAlignment: Text.AlignLeft;
                    verticalAlignment: Text.AlignBottom;
                }

                PhxScrollView {
                    Layout.fillWidth: true;
                    Layout.preferredHeight: 30;
                    Layout.alignment: Qt.AlignTop | Qt.AlignLeft;
                    Flickable {
                        anchors.fill: parent;
                        contentWidth: numButtons * ( 30 + 10 );
                        contentHeight: 30;
                        Row {
                            anchors.fill: parent;
                            spacing: 10;
                            Repeater {
                                model: numButtons;
                                delegate: Rectangle {
                                    width: 30;
                                    height: 30;
                                    color: button[ index ] ? PhxTheme.common.successColor : PhxTheme.common.errorColor;
                                    Text {
                                        anchors.fill: parent;
                                        anchors.centerIn: parent;
                                        color: button[ index ] ? "white" : Qt.darker( "white" );
                                        text: index;
                                        horizontalAlignment: Text.AlignHCenter;
                                        verticalAlignment: Text.AlignVCenter;
                                    }
                                }
                            }
                        }
                    }
                }

                PhxScrollView {
                    Layout.fillWidth: true;
                    Layout.preferredHeight: 20;
                    Layout.alignment: Qt.AlignTop | Qt.AlignLeft;
                    Flickable {
                        anchors.fill: parent;
                        contentWidth: numAxes * ( 120 + 10 );
                        contentHeight: 20;
                        Row {
                            anchors.fill: parent;
                            spacing: 10;
                            Button {
                                text: "Detect center";
                                width: 120;
                                height: 20;
                                onClicked: {
                                    for( var i = 0; i < numAxes; i++ ) {
                                        if( Math.abs( axis[ i ] ) > 30000 ) {
                                            remapperModel.setDeadzone( currentGUID, i, deadzone[ i ], false );
                                        } else {
                                            remapperModel.setDeadzone( currentGUID, i, deadzone[ i ], true );
                                        }
                                    }
                                }
                            }

                            Repeater {
                                model: numAxes;
                                delegate: Item {
                                    width: 120;
                                    height: 20;
                                    property bool activated: Math.abs( axis[ index ] + ( deadzoneMode[ index ] ? 0 : 32768 ) ) > deadzone[ index ];
                                    Rectangle {
                                        anchors.left: parent.left;
                                        anchors.top: parent.top;
                                        anchors.bottom: parent.bottom;
                                        color: activated ? PhxTheme.common.successColor : PhxTheme.common.errorColor;
                                        width: ( ( axis[ index ] + 0x7FFF ) / 0xFFFF ) * parent.width;
                                    }
                                    Rectangle {
                                        anchors.fill: parent;
                                        color: "transparent";
                                        border.color: activated ? "white" : Qt.darker( "white" );
                                        border.width: 2;
                                    }
                                    Text {
                                        anchors.fill: parent;
                                        anchors.centerIn: parent;
                                        text: "Analog " + index + ": " + axis[ index ];
                                        color: activated ? "white" : Qt.darker( "white" );
                                        horizontalAlignment: Text.AlignHCenter;
                                        verticalAlignment: Text.AlignVCenter;
                                    }
                                }
                            }
                        }
                    }
                }

                PhxScrollView {
                    Layout.fillWidth: true;
                    Layout.preferredHeight: 50;
                    Layout.alignment: Qt.AlignTop | Qt.AlignLeft;
                    Flickable {
                        anchors.fill: parent;
                        contentWidth: numHats * ( 50 + 10 );
                        contentHeight: 50;
                        Row {
                            anchors.fill: parent;
                            spacing: 10;
                            Repeater {
                                model: numHats;
                                delegate: Item {
                                    width: 50;
                                    height: 50;

                                    Timer {
                                        interval: 1;
                                        repeat: true;
                                        running: true;
                                        onTriggered: canvas.requestPaint();
                                    }

                                    Canvas {
                                        id: canvas;
                                        anchors.fill: parent;
                                        property int crossSize: width * ( 1 / 3 );
                                        property int crossLabelSize: width / 5;
                                        property int crossLabelPadding: width / 10;
                                        property color fillColor: "black";
                                        property color strokeColor: "grey";

                                        onPaint: {
                                            var ctx = getContext( "2d" );
                                            ctx.fillStyle = fillColor;
                                            ctx.strokeStyle = strokeColor;
                                            ctx.lineWidth = 2;

                                            // Clear previous frame
                                            ctx.clearRect( 0, 0, width, height );

                                            // Set up cross path
                                            ctx.beginPath();
                                            ctx.moveTo( ( width - crossSize ) / 2, ( height - crossSize ) / 2 );
                                            ctx.lineTo( ( width - crossSize ) / 2, 0 );
                                            ctx.lineTo( ( ( width - crossSize ) / 2 ) + crossSize, 0 );
                                            ctx.lineTo( ( ( width - crossSize ) / 2 ) + crossSize, ( height - crossSize ) / 2 );

                                            ctx.lineTo( width, ( height - crossSize ) / 2 );
                                            ctx.lineTo( width, ( ( height - crossSize ) / 2 ) + crossSize );
                                            ctx.lineTo( ( ( width - crossSize ) / 2 ) + crossSize, ( ( height - crossSize ) / 2 ) + crossSize );

                                            ctx.lineTo( ( ( width - crossSize ) / 2 ) + crossSize, height );
                                            ctx.lineTo( ( width - crossSize ) / 2, height );
                                            ctx.lineTo( ( width - crossSize ) / 2, ( ( height - crossSize ) / 2 ) + crossSize );

                                            ctx.lineTo( 0, ( ( height - crossSize ) / 2 ) + crossSize );
                                            ctx.lineTo( 0, ( height - crossSize ) / 2 );
                                            //ctx.lineTo( ( width - crossSize ) / 2, ( height - crossSize ) / 2 );

                                            ctx.closePath();

                                            // Draw cross
                                            ctx.fill();
                                            ctx.stroke();

                                            // Don't draw anything else if we're centered
                                            if( hat[ index ] !== 0 ) {
                                                // Find angle to rotate canvas by
                                                // Check diagonals first
                                                var angle = 0;

                                                var tempPadding = crossLabelPadding;

                                                // NW
                                                if( hat[ index ] === ( 0x1 | 0x8 ) ) {
                                                    angle = Math.PI * ( 7 / 4 );
                                                    tempPadding -= width / 10;
                                                }

                                                // SW
                                                else if( hat[ index ] === ( 0x4 | 0x8 ) ) {
                                                    angle = Math.PI * ( 5 / 4 );
                                                    tempPadding -= width / 10;
                                                }

                                                // SE
                                                else if( hat[ index ] === ( 0x4 | 0x2 ) ) {
                                                    angle = Math.PI * ( 3 / 4 );
                                                    tempPadding -= width / 10;
                                                }

                                                // NE
                                                else if( hat[ index ] === ( 0x1 | 0x2 ) ) {
                                                    angle = Math.PI * ( 1 / 4 );
                                                    tempPadding -= width / 10;
                                                }

                                                // N
                                                else if( hat[ index ] === ( 0x1 ) ) {
                                                    angle = Math.PI * ( 0 / 2 );
                                                }

                                                // S
                                                else if( hat[ index ] === ( 0x4 ) ) {
                                                    angle = Math.PI * ( 2 / 2 );
                                                }

                                                // E
                                                else if( hat[ index ] === ( 0x2 ) ) {
                                                    angle = Math.PI * ( 1 / 2 );
                                                }

                                                // W
                                                else if( hat[ index ] === ( 0x8 ) ) {
                                                    angle = Math.PI * ( 3 / 2 );
                                                }

                                                // Rotate the canvas by the angle
                                                ctx.translate( width / 2, height / 2 );
                                                ctx.rotate( angle );
                                                ctx.translate( -width / 2, -height / 2 );

                                                // Draw equilateral triangle
                                                ctx.beginPath();
                                                ctx.moveTo( width / 2, tempPadding );
                                                ctx.lineTo( ( width / 2 ) + ( Math.cos( ( 10 * Math.PI ) / 6 ) * crossLabelSize ),
                                                            tempPadding - ( Math.sin( ( 10 * Math.PI ) / 6 ) * crossLabelSize ) );
                                                ctx.lineTo( ( width / 2 ) + ( Math.cos( ( 10 * Math.PI ) / 6 ) * crossLabelSize ) - crossLabelSize,
                                                            tempPadding - ( Math.sin( ( 10 * Math.PI ) / 6 ) * crossLabelSize ) );
                                                ctx.closePath();
                                                ctx.fillStyle = strokeColor;
                                                ctx.fill();

                                                // Rotate back to original position
                                                ctx.resetTransform();
                                            }
                                        }
                                    }

                                    Text {
                                        anchors.fill: parent;
                                        color: "white";
                                        text: index + " " + hat[ index ];
                                        horizontalAlignment: Text.AlignHCenter;
                                        verticalAlignment: Text.AlignVCenter;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
