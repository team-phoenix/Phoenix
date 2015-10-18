import QtQuick 2.5

Rectangle {
    id:tooltip;
    property alias text: textContainer.text;
    property int verticalPadding: 8;
    property int horizontalPadding: 8;
    color: "#75000000";
    width: textContainer.width + horizontalPadding * 2;
    height: textContainer.height + verticalPadding * 2;
    z: 9999999999;
    anchors { horizontalCenter: parent.horizontalCenter }

    Text {
        anchors.centerIn: parent;
        id:textContainer;
        text: "Tooltip";
        color: "#95FFFFFF";
    }

    NumberAnimation {
        id: fadein;
        target: tooltip;
        property: "opacity";
        easing.type: Easing.InOutQuad;
        duration: 300;
        from: 0;
        to: 1;
    }

    NumberAnimation {
        id: fadeout;
        target: tooltip;
        property: "opacity";
        easing.type: Easing.InOutQuad;
        from: 1;
        to: 0;
        onStopped: visible = false;
    }

    visible:false;
    onVisibleChanged: if(visible)fadein.start();

    function show(){ visible = true; fadein.start(); }
    function hide(){ fadeout.start(); }
}
