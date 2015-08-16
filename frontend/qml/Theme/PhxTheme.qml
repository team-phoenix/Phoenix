import QtQuick 2.3

pragma Singleton

QtObject {
    property color normalFontColor: "#f1f1f1";

    property QtObject selectionArea: QtObject {
        property int basePixelSize: 12;
        property int headerFontSize: 13;
        property color baseColor: "#2e0e19";
        property color baseFontColor: "#8a8889";
        property color highlightFontColor: "#ec1a23";
    }

    property QtObject common: QtObject {
        property color baseFontColor: "white";
    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1f1f1f";
        property color highlightColor: "blue";
    }

}

