import QtQuick 2.3

pragma Singleton

QtObject {
    property color normalFontColor: "#f1f1f1";

    property QtObject sectionArea: QtObject {
        property int normalFontSize: 12;
        property int headerFontSize: 13;
    }

    property QtObject common: QtObject {
        property color baseFontColor: "black";
    }

}

