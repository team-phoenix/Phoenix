import QtQuick 2.3

Rectangle {
    id: customBorder;

    property bool shareMargin: true;

    property int _topMargin: 1;
    property int _leftMargin: 1;
    property int _rightMargin: 1;
    property int _bottomMargin: 1;
    property int _margin: 1;

    radius: parent.radius;
    color: "blue";

    z: parent.z - 1;

    anchors {
        fill: customBorder.parent;
        topMargin: shareMargin ? -_margin : -_topMargin;
        bottomMargin: shareMargin ?- _margin : -customBorder._bottomMargin;
        leftMargin: shareMargin ? -_margin : -customBorder._leftMargin;
        rightMargin: shareMargin ? -_margin : -customBorder._rightMargin;
    }
}
