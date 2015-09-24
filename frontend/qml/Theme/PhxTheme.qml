import QtQuick 2.3

pragma Singleton

QtObject {
    property color normalFontColor: "#f1f1f1";

    property QtObject selectionArea: QtObject {
        property int basePixelSize: 12;
        property int headerFontSize: 14;
        property color baseColor: "#2e0e19";
        property color baseFontColor: "#d5d5d5" // "#8a8889";
        property color highlightFontColor: "#ffffff" // "#d5d5d5";
    }

    property QtObject common: QtObject {
        property color baseFontColor: "#ADADAD";
        property color highlighterFontColor: "#D5D5D5";
        property color primaryBackgroundColor: "#1F1F1F";
        property color secondaryBackgroundColor: "#0A0A0A";

        property color boxartNormalBorderColor: "#0E0E0E";
        property color boxartSelectedBorderColor: "#FFFFFF";

        property color gameViewBackgroundColor: "#1F1F1F";

        property Gradient primaryButtonColor: Gradient {
            GradientStop { position: 0.0; color: "#e97f54"; }
            GradientStop { position: 0.3; color: "#d55b4a"; }
            GradientStop { position: 1.0; color: "#bf333f"; }
        }
        property color normalButtonColor: "#2e2e2e";

    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1f1f1f";
        property color highlightColor: "blue";
    }

}

