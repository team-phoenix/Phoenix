import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.1
import QtQuick.Window 2.0
import Qt.labs.settings 1.0
import phoenix.window 1.0
import phoenix.library 1.0
import QtQuick.Window 2.0

PhoenixWindow {
    id: root;
    width: Screen.width / 2;
    height: Screen.height / 2;
    minimumHeight: 480;
    minimumWidth: 640;
    //swapInterval: 0;
    frameless: false;
    title: "Phoenix";

    property string infoBarText: "";
    property bool clear: false//(phoenixLibrary.count === 0);
    property string accentColor:"#e8433f";
    property int lastWindowStyle: Window.Windowed;
    property string borderColor: "#0b0b0b";
    property string flagValue: "";
    property bool gameShowing: false;
    property string systemDirectory: "";
    property string saveDirectory: "";
    property real volumeLevel: 1.0;
    property string prevView: "";
    property bool ranOnce: false;
    property bool screenTimer: false;
    property int filtering: 2;
    property bool stretchVideo: false;
    property string itemInView: "grid";

    onGameShowingChanged: {
        if (gameShowing) {
            headerBar.previousViewIcon = headerBar.viewIcon;
            headerBar.viewIcon = "../assets/GameView/home.png";
        }
        else {
            headerBar.viewIcon = headerBar.previousViewIcon;
        }
    }

    onWidthChanged: {
        settingsDropDown.state = "retracted";
    }


    function swapScreenSize(){
        if (root.visibility == Window.FullScreen)
            root.visibility = lastWindowStyle;
        else {
            lastWindowStyle = root.visibility;
            root.visibility = Window.FullScreen;
        }
    }

    Rectangle {
        id: leftBorder;
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
        }
        width: 1;
        color: borderColor;
    }

    Rectangle {
        id: rightBorder;
        anchors {
            top: parent.top;
            right: parent.right;
            bottom: parent.bottom;
        }
        width: 1;
        color: borderColor;
    }

    Rectangle {
        id: bottomBorder;
        anchors {
            bottom: bottom.top;
            left: parent.left;
            right: parent.right;
        }
        height: 1;
        color: borderColor;
    }

    PhoenixLibrary {
        id: phoenixLibrary;
    }

    MouseArea {
        anchors.fill: parent;
        enabled: settingsDropDown.visible;
        onClicked: settingsDropDown.visible = false;
    }

    Component {
        id: gameGrid;

        Item {
            id: backdropGrid;
            property string actionColor: "#e8433f";
            property int borderWidth: 5;
            property bool showBorder: false;
        Rectangle {
            id: actionBorderLeft;
            z: grid.z + 1;
            visible: parent.showBorder;
            color: parent.actionColor;
            anchors {
                top: parent.top;
                topMargin: 1;
                bottomMargin: 1;
                bottom: parent.bottom;
                left: parent.left;
            }
            width: parent.borderWidth;

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "#f27b77";
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    right: parent.right;
                }
                height: 1;
                color: "#f27b77";
            }

            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: 1;
                color: "#f27b77";
            }

        }

        Rectangle {
            id: actionBorderTop;
            color: parent.actionColor;
            z: grid.z + 1;
            visible: parent.showBorder;
            anchors {
                top: parent.top;
                topMargin: 1;
                left: actionBorderLeft.right;
                right: parent.right;
                rightMargin: 1;
            }
            height: parent.borderWidth;

            Rectangle {
                anchors {
                    left: parent.left;
                    top: parent.top;
                    right: parent.right;
                }
                height: 1;
                color: "#f27b77";
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom;
                    top: parent.top;
                    right: parent.right;
                }
                width: 1;
                color: "#f27b77";
            }
        }

        Rectangle {
            id: actionBorderRight;
            color: parent.actionColor;
            visible: parent.showBorder;
            z: grid.z + 1;
            anchors {
                top: actionBorderTop.bottom;
                bottom: actionBorderBottom.top;
                right: parent.right;
                rightMargin: 1;
            }
            width: parent.borderWidth

            Rectangle {
                anchors {
                    bottom: parent.bottom;
                    top: parent.top;
                    right: parent.right;
                }
                width: 1;
                color: "#f27b77";
            }
        }

        Rectangle {
            id: actionBorderBottom;
            color: parent.actionColor;
            visible: parent.showBorder;
            z: grid.z + 1;
            anchors {
                bottom: parent.bottom;
                bottomMargin: 1;
                left: actionBorderLeft.right;
                right: parent.right;
                rightMargin: 1;
            }
            height: parent.borderWidth;

            Rectangle {
                anchors {
                    left: parent.left;
                    bottom: parent.bottom;
                    right: parent.right;
                }
                height: 1;
                color: "#f27b77";
            }

            Rectangle {
                anchors {
                    bottom: parent.bottom;
                    top: parent.top;
                    right: parent.right;
                }
                width: 1;
                color: "#f27b77";
            }
        }

        GameGrid {
            id: grid;

            DropArea {
                anchors.fill: parent;
                onEntered: {
                    backdropGrid.showBorder = true;
                    phoenixLibrary.cacheUrls(drag.urls);
                }

                onDropped: {
                    phoenixLibrary.importUrls = true;
                    backdropGrid.showBorder = false;
                }
                onExited:  backdropGrid.showBorder = false;
            }

            //MouseArea {
               // anchors.fill: parent;
                //propagateComposedEvents: true;
                //hoverEnabled: true;
                //onEntered: {
                //    backdropGrid.showBorder = true;
                //}

                //onExited: backdropGrid.showBorder = false;
            //}
            property string itemName: "grid";
            color: "#262626";
            zoomFactor: headerBar.sliderValue;
            zoomSliderPressed: headerBar.sliderPressed;
            anchors.fill: parent;
            //height: parent.height;
            //width: parent.width;
            Behavior on height {
                PropertyAnimation {}
            }

            Behavior on width {
                PropertyAnimation {}
            }
        }
        }
    }

    Component {
        id: gameTable;
        Item {
            id: backdropGrid;
            property string actionColor: "#e8433f";
            property int borderWidth: 5;
            property bool showBorder: false;
            Rectangle {
                id: actionBorderLeft;
                z: table.z + 1;
                visible: parent.showBorder;
                color: parent.actionColor;
                anchors {
                    top: parent.top;
                    topMargin: 1;
                    bottomMargin: 1;
                    bottom: parent.bottom;
                    left: parent.left;
                }
                width: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        bottom: parent.bottom;
                    }
                    width: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    height: 1;
                    color: "#f27b77";
                }

            }

            Rectangle {
                id: actionBorderTop;
                color: parent.actionColor;
                z: table.z + 1;
                visible: parent.showBorder;
                anchors {
                    top: parent.top;
                    topMargin: 1;
                    left: actionBorderLeft.right;
                    right: parent.right;
                    rightMargin: 1;
                }
                height: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }

            Rectangle {
                id: actionBorderRight;
                color: parent.actionColor;
                visible: parent.showBorder;
                z: table.z + 1;
                anchors {
                    top: actionBorderTop.bottom;
                    bottom: actionBorderBottom.top;
                    right: parent.right;
                    rightMargin: 1;
                }
                width: parent.borderWidth

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }

            Rectangle {
                id: actionBorderBottom;
                color: parent.actionColor;
                visible: parent.showBorder;
                z: table.z + 1;
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    left: actionBorderLeft.right;
                    right: parent.right;
                    rightMargin: 1;
                }
                height: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        bottom: parent.bottom;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }
            GameTable {
                id: table;
                itemName: "table";
                highlightColor: "#4a4a4a";
                textColor: "#f1f1f1";
                headerColor: "#262626";
                anchors.fill: parent;
                DropArea {
                    anchors.fill: parent;
                    onEntered: {
                        backdropGrid.showBorder = true;
                        phoenixLibrary.cacheUrls(drag.urls);
                    }

                    onDropped: {
                        phoenixLibrary.importUrls = true;
                        backdropGrid.showBorder = false;
                    }
                    onExited:  backdropGrid.showBorder = false;
                }
            }
        }
    }

    Settings {
        id: settings;
        category: "UI";
        //property alias windowX: root.x;
        //property alias windowY: root.y;
        //property alias windowWidth: root.width;
        //property alias windowHeight: root.height;
        property alias volumeLevel: root.volumeLevel;
        property alias smooth: root.filtering;
        property alias stretchVideo: root.stretchVideo;
    }

    HeaderBar {
        id: headerBar;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        Behavior on height {
            NumberAnimation {
                duration: 150;
            }
        }

        height: 55;
        //color: "#3b3b3b";
        fontSize: 14;
    }

    InfoBar {
        id: infoBar;
        anchors {
            left: parent.left;
            right: parent.right;
            top: headerBar.bottom;
            leftMargin: 1;
            rightMargin: 1;
        }
        z: headerBar.z + 1;
    }

    DropShadow {
        visible: infoBar.visible;
        anchors.fill: source;
        source: infoBar;
        horizontalOffset: 0
        verticalOffset: 6
        radius: 8.0
        samples: 16
        color: "#80000000"
        transparentBorder: true;
    }

    SettingsDropDown {
        id: settingsDropDown;
        z: headerBar.z + 1;
        visible: false;
        opacity: visible ? 1.0 : 0.0;

        Behavior on opacity {
            NumberAnimation {
                easing {
                    type: Easing.OutQuad;
                }
                duration: 200;
            }
        }

        anchors {
            top: headerBar.bottom;
            topMargin: 3;
            left: parent.left;
            leftMargin: 10;
        }
        height: 200;
        width: 125;

        stackBackgroundColor: "#f4f4f4";
        contentColor: "#f4f4f4";
        textColor: "#f1f1f1";

        Rectangle {
            visible: parent.visible;
            opacity: parent.visible ? 1.0 : 0.0;
            height: 15;
            width: 15;
            rotation: 45;
            color: "#2f2f2f";
            z: settingsDropDown.z + 1;
            anchors {
                left: parent.left;
                leftMargin: 16;
                verticalCenter: settingsDropDown.top;
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    leftMargin: 1;
                    right: parent.right;
                }
                height: 1;
                color: "#333333";
            }

            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    bottom: parent.bottom;
                }
                width: 1;
                color: "#333333";
            }

            Behavior on opacity {
                NumberAnimation {
                    easing {
                        type: Easing.OutQuad;
                    }
                    duration: 200;
                }
            }



        }

    }

    RectangularGlow {
        anchors.fill: settingsDropDown;
        visible: settingsDropDown.visible;
        color: "#80000000";

                glowRadius: 10
                spread: 0;
                cornerRadius: 6;
    }


    StackView {
        id: windowStack;
        z: headerBar.z - 1;

        height: headerBar.visible ? (parent.height - headerBar.height) : (parent.height);
        anchors {
            top: (currentItem !== null && currentItem.stackName == "gameview") ? headerBar.top : headerBar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        initialItem: homeScreen;

        property string gameStackItemName: {
            if (currentItem != null && typeof currentItem.stackName !== "undefined") {
                return currentItem.stackName;
            }
            else {
                return "";
            }
        }

        delegate: StackViewDelegate {
            function transitionFinished(properties)
            {
                properties.exitItem.x = 0;
            }
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem;
                    property: "x";
                    from: -enterItem.width;
                    to: 0;
                    duration: 600;
                }
                PropertyAnimation {
                    target: exitItem;
                    property: "x";
                    from: 0;
                    to: -exitItem.width;
                    duration: 600;
                }
            }
       }
    }

    Component {
        id: gameView;
        GameView {
        }
    }

    Component {
        id: homeScreen;

        Item {
            id: backdropGrid;
            property string stackName: "homescreen";
            property StackView stackId: gameStack;

            property string actionColor: "#e8433f";
            property int borderWidth: 5;
            property bool showBorder: false;

                    //Rectangle {
                      //  anchors {
                        //    fill: parent;
                       // }
                       // opacity: 0.1;



                       // color: "#f27b77";
                    //}


            Rectangle {
                id: actionBorderLeft;
                z: consoleBar.z + 1;
                visible: parent.showBorder;
                color: parent.actionColor;
                anchors {
                    top: parent.top;
                    topMargin: 1;
                    bottomMargin: 1;
                    bottom: parent.bottom;
                    left: parent.left;
                }
                width: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        bottom: parent.bottom;
                    }
                    width: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    height: 1;
                    color: "#f27b77";
                }

            }

            Rectangle {
                id: actionBorderTop;
                color: parent.actionColor;
                z: consoleBar.z + 1;
                visible: parent.showBorder;
                anchors {
                    top: parent.top;
                    topMargin: 1;
                    left: actionBorderLeft.right;
                    right: consoleBar.right;
                    rightMargin: 1;
                }
                height: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }

            Rectangle {
                id: actionBorderRight;
                color: parent.actionColor;
                visible: parent.showBorder;
                z: consoleBar.z + 1;
                anchors {
                    top: actionBorderTop.bottom;
                    bottom: actionBorderBottom.top;
                    right: consoleBar.right;
                    rightMargin: 1;
                }
                width: parent.borderWidth

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }

            Rectangle {
                id: actionBorderBottom;
                color: parent.actionColor;
                visible: parent.showBorder;
                z: consoleBar.z + 1;
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: 1;
                    left: actionBorderLeft.right;
                    right: consoleBar.right;
                    rightMargin: 1;
                }
                height: parent.borderWidth;

                Rectangle {
                    anchors {
                        left: parent.left;
                        bottom: parent.bottom;
                        right: parent.right;
                    }
                    height: 1;
                    color: "#f27b77";
                }

                Rectangle {
                    anchors {
                        bottom: parent.bottom;
                        top: parent.top;
                        right: parent.right;
                    }
                    width: 1;
                    color: "#f27b77";
                }
            }

            ConsoleBar {
                id: consoleBar;
                z: headerBar.z - 1;
                color: "#262626";
                anchors {
                    left: parent.left;
                    top: parent.top;
                    bottom: parent.bottom;
                }
                width: 200;

                DropArea {
                    anchors.fill: parent;
                    onEntered: {
                        backdropGrid.showBorder = true;
                        phoenixLibrary.cacheUrls(drag.urls);
                    }

                    onDropped: {
                        phoenixLibrary.importUrls = true;
                        backdropGrid.showBorder = false;
                    }
                    onExited:  backdropGrid.showBorder = false;
                }


            }

            StackView {
                id: gameStack;
                z: windowStack.z;

                initialItem: {
                    if (root.clear === true)
                        return emptyScreen;

                    return gameGrid;
                }

                anchors {
                    left: consoleBar.right;
                    right: parent.right;
                    top: parent.top;
                    bottom: parent.bottom;
                }

            }

            Component {
                id: emptyScreen;

                Rectangle {
                    property string itemName: "empty";
                    color: "#1d1e1e";

                    Column {
                        anchors.centerIn: parent;
                        spacing: 30;

                        Column {
                            //anchors.horizontalCenter: parent.horizontalCenter;
                            spacing: 2;
                            Label {
                                anchors.horizontalCenter: parent.horizontalCenter;
                                text: "Get Some Games"
                                color: "#f1f1f1";
                                font {
                                    family: "Sans";
                                    pixelSize: 26;
                                }
                                horizontalAlignment: Text.AlignHCenter;
                            }

                            Label {

                                text: "Phoenix can't seem to find your games."
                                color: "gray";
                                font {
                                    family: "Sans";
                                    pixelSize: 16;
                                }
                                horizontalAlignment: Text.AlignHCenter;
                            }

                        }


                        FileDialog {
                            id: pathDialog;
                            selectFolder: true;
                            title: "Add Game Folder";
                            onAccepted: {
                                phoenixLibrary.startAsyncScan(fileUrl);
                            }
                        }

                        Button {
                            id: importGamesBtn;
                            onClicked: pathDialog.open();
                            property string backgroundColor: "#000000FF";
                            onHoveredChanged: {
                                if (hovered) {
                                    backgroundColor = "#525252";
                                }
                                else
                                    backgroundColor = "#000000FF";
                            }

                            style: ButtonStyle {
                                background: Rectangle {
                                    color: importGamesBtn.backgroundColor;
                                }

                                label: Row {
                                    spacing: 25;
                                    Image {
                                        source: "../assets/folder-8x.png"
                                        height: 40;
                                        width: 40;
                                        sourceSize {
                                            width: 40;
                                            height: 40;
                                        }
                                    }

                                    Column {
                                        anchors.verticalCenter: parent.verticalCenter;
                                        spacing: 2;
                                        Label {
                                            text: "Add Games";
                                            color: "#f1f1f1";
                                            font {
                                                family: "Sans";
                                                pixelSize: 18;
                                                bold: true;
                                            }
                                        }

                                        Label {
                                            text: "Choose your games folder.";
                                            color: "gray";
                                            font {
                                                family: "Sans";
                                                pixelSize: 16;
                                            }
                                        }
                                    }

                                }

                            }
                        }

                        Button {
                            id: importLibrarBtn;
                            property string backgroundColor: "#000000FF";
                            onClicked: {
                                pathDialog.selectFolder = false;
                                pathDialog.title = "Import Library File";
                                pathDialog.nameFilters = ["Library file (*.json)", "All files (*)"];
                                pathDialog.open();
                            }
                            onHoveredChanged: {
                                if (hovered) {
                                    backgroundColor = "#525252";
                                }
                                else
                                    backgroundColor = "#000000FF";
                            }

                            style: ButtonStyle {
                                background: Rectangle {
                                    //opacity: 0.3;
                                    color: importLibrarBtn.backgroundColor;
                                }
                                label: Row {
                                    spacing: 25;
                                    Image {
                                        source: "../assets/file-8x.png";
                                        height: 40;
                                        width: 40;
                                        sourceSize {
                                            width: 40;
                                            height: 40;
                                        }
                                    }

                                    Column {
                                        anchors.verticalCenter: parent.verticalCenter;
                                        spacing: 2;
                                        Label {
                                            text: "Import Library";
                                            color: "#f1f1f1";
                                            font {
                                                family: "Sans";
                                                pixelSize: 18;
                                            }
                                        }
                                        Label {
                                            text: "Add from a Hard Drive.";
                                            color: "gray";
                                            font {
                                                family: "Sans";
                                                pixelSize: 16;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
