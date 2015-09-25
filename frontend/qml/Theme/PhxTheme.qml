import QtQuick 2.3

pragma Singleton

QtObject {
    property color normalFontColor: "#F1F1F1";

    property QtObject selectionArea: QtObject {
        property int basePixelSize: 12;
        property int headerFontSize: 15;
        property color baseColor: "#2E0E19";
        property color baseFontColor: "#D5D5D5" // "#8A8889";
        property color highlightFontColor: "#FFFFFF" // "#D5D5D5";
    }

    property QtObject common: QtObject {
        property color baseFontColor: "#ADADAD";
        property color highlighterFontColor: "#D5D5D5";

        property color primaryBackgroundColor: "#202028";
        property color secondaryBackgroundColor: "#1a1a20";

        property color highlightBackgroundColor: "#121216";
        property color highlightHoverBackgroundColor: "#18181d";

        property color boxartNormalBorderColor: "#464854";
        property color boxartSelectedBorderColor: "#FFFFFF";

        property color gameViewBackgroundColor: "#1F1F1F";

        property Gradient primaryButtonColor: Gradient {
            GradientStop { position: 0.0; color: "#E97F54"; }
            GradientStop { position: 0.3; color: "#D55b4A"; }
            GradientStop { position: 1.0; color: "#BF333F"; }
        }
        property color normalButtonColor: "#464854";

    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1F1F1F";
        property color highlightColor: "blue";
    }

}

