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
        property int menuWidth: 320;
        property color baseFontColor: "#ADADAD";
        property color highlighterFontColor: "#D5D5D5";

        property string backgroundImage: "";

        property color baseBackgroundColor: "#171717";
        property color primaryBackgroundColor: "#00000000";
        property color secondaryBackgroundColor: "#75000000";

        property int suspendedGameFontSize: 13;
        property color suspendedGameBackgroundColor: "#55343C3F";
        property color suspendedGameHoverBackgroundColor: "#25FFFFFF";

        property color boxartNormalBorderColor: "#20FFFFFF";
        property color boxartSelectedBorderColor: "#FFFFFF";

        property color gameViewBackgroundColor: "#1F1F1F";

        property Gradient primaryButtonColor: Gradient {
            GradientStop { position: 0.0; color: "#75FFFFFF"; }
            GradientStop { position: 0.5; color: "#20FFFFFF"; }
            GradientStop { position: 1.0; color: "#00FFFFFF"; }
        }
        property color normalButtonColor: "#19FFFFFF";

    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1F1F1F";
        property color highlightColor: "blue";
    }

}

