import QtQuick 2.3
pragma Singleton

QtObject {
    property color normalFontColor: "#F1F1F1";

    property QtObject selectionArea: QtObject {
        property int basePixelSize:        12;
        property int headerFontSize:       15;
        property color baseColor:          "#2E0E19";
        property color baseFontColor:      "#D5D5D5" // "#8A8889";
        property color highlightFontColor: "#FFFFFF" // "#D5D5D5";
    }

    property QtObject common: QtObject {
        property int menuWidth:               250;
        property color baseFontColor:        "#ADADAD";
        property color highlighterFontColor: "#D5D5D5";

        property string backgroundImage: "bg.png";

        property color baseBackgroundColor:      "#43060D";
        property color primaryBackgroundColor:   "#50000000";
        property color secondaryBackgroundColor: "#00000000";

        property int suspendedGameFontSize: 13;
        property color suspendedGameBackgroundColor:      "#25FFFFFF";
        property color suspendedGameHoverBackgroundColor: "#15FFFFFF";

        property color boxartNormalBorderColor:   "#00FFFFFF";
        property color boxartSelectedBorderColor: "#75FFFFFF";

        property color gameViewBackgroundColor: "black";

        property color normalButtonColor: "#19FFFFFF";
    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1F1F1F";
        property color highlightColor: "blue";
    }
}
