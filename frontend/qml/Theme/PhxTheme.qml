import QtQuick 2.3
import QtQuick.Controls 1.4
pragma Singleton

QtObject {
    property color normalFontColor: "#F1F1F1";

    property QtObject selectionArea: QtObject {
        property int basePixelSize:        13;
        property int headerFontSize:       13;
        property color baseColor:          "#2E0E19";
        property color baseFontColor:      "#75FFFFFF" // "#8A8889";
        property color highlightFontColor: "#FFFFFF" // "#D5D5D5";
    }

    property QtObject common: QtObject {
        property string systemFontFamily: dummyLabel.font.family;
        property int baseFontSize:    13;
        property color baseFontColor:        "#ADADAD";
        property color highlighterFontColor: "#FFFFFF";

        property int menuWidth:       250;
        property int menuTitleHeight: 50;
        property int menuItemHeight:  30;
        property color menuTitleColor: "#35000000";
        property color menuItemHighlight: "#A32127";
        property color menuSelectedColor: "#FFFFFF";

        property string backgroundImage: "bg.png";

        property color baseBackgroundColor:      "#FFFFFF";
        property color primaryBackgroundColor:   "#14151A";
        property color secondaryBackgroundColor: "#201F27";

        property int suspendedGameFontSize: 13;
        property color gameSuspendedBackgroundColor:      "#A32127";
        property color gameSuspendedHoverBackgroundColor: "#15FFFFFF";

        property int boxartFontSize: 12;
        property color boxartNormalBorderColor:   "#00FFFFFF";
        property color boxartSelectedBorderColor: "#55AAAAAA";

        property color gameViewBackgroundColor: "#000000";

        property color normalButtonColor: "#10FFFFFF";
    }

    property QtObject bigPictureView: QtObject {
        property color baseColor: "#1F1F1F";
        property color highlightColor: "blue";
    }

    // Small hack to get the system's font
    property Label dummyLabel: Label {
        Component.onCompleted: {
            console.log( "PhxTheme: Using system font \"" + font.family + "\"" );
        }
    }
}
