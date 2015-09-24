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
        property color baseFontColor: "#adadad";
        property color highlighterFontColor: "#d5d5d5";
        property color primaryBackgroundColor: "#202028";

        property Gradient primaryButtonColor: Gradient {
            GradientStop { position: 0.0; color: "#e97f54"; }
            GradientStop { position: 0.3; color: "#d55b4a"; }
            GradientStop { position: 1.0; color: "#bf333f"; }
        }
        property Gradient normalButtonColor: Gradient {
            GradientStop { position: 0.0; color: "#616274"; }
            //GradientStop { position: 0.0; color: "#616274"; }
            GradientStop { position: 1.0; color: "#3e404d"; }
        }

    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1f1f1f";
        property color highlightColor: "blue";
    }

}

