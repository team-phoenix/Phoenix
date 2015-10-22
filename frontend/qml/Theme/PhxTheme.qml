import QtQuick 2.3
import QtQuick.Controls 1.4
pragma Singleton

QtObject {
    property color normalFontColor: "#F1F1F1";

    property QtObject selectionArea: QtObject {
        property int headerFontSize:       20;
        property color baseColor:          "#2E0E19";
        property color baseFontColor:      "#75FFFFFF" // "#8A8889";
        property color highlightFontColor: "#FFFFFF" // "#D5D5D5";
    }

    property QtObject common: QtObject {
        // BASE
        property string systemFontFamily: dummyLabel.font.family;
        property int baseFontSize:    12;
        property color baseFontColor:        "#ADADAD";
        property color highlighterFontColor: "#FFFFFF";

        // MENU
        property int menuWidth:       250;
        property int menuTitleHeight: 70;
        property int menuItemHeight:  35;
        property color menuTitleColor: "#35000000";
        property color menuItemHighlight: "#A32127";
        property color menuItemBackgroundColor: "#A32127";
        property color menuSelectedColor: "#FFFFFF";

        // BACKGROUND
        property string backgroundImage: "";
        property color baseBackgroundColor:      "#FFFFFF"; // Black or White
        property color primaryBackgroundColor:   "#14151A"; // Sidebar
        property color secondaryBackgroundColor: "#201F27"; // Content (Grid)

        // SUSPENDED GAME AREA
        property int suspendedGameFontSize: 13;
        property color gameSuspendedBackgroundColor:      "#A32127";
        property color gameSuspendedHoverBackgroundColor: "#15FFFFFF";

        // BOXART
        property int boxartFontSize: 12;
        property color boxartNormalBorderColor:   "#00FFFFFF";
        property color boxartSelectedBorderColor: "#55AAAAAA";

        property color gameViewBackgroundColor: "#000000";
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
