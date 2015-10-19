import QtQuick 2.4
import QtQuick.Controls.Private 1.0

// NativeTooltip.qml
// This file contains private Qt Quick modules that might change in future versions of Qt
// Tested on: Qt 5.4.1, 5.5.1
// Source: https://www.kullo.net/blog/tooltiparea-the-missing-tooltip-component-of-qt-quick/

MouseArea {
    id: _root;
    property string text: "";

    anchors.fill: parent;
    hoverEnabled: _root.enabled;

    onExited: Tooltip.hideText();
    onCanceled: Tooltip.hideText();

    onClicked: {
        if( typeof( _root.parent.pressed ) !== "undefined" )
            _root.parent.pressed = !_root.parent.pressed;
    }

    Timer {
        interval: 1000;
        running: _root.enabled && _root.containsMouse && _root.text.length;
        onTriggered: Tooltip.showText( _root, Qt.point( _root.mouseX, _root.mouseY ), _root.text );
    }
}
