import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import phoenix.video 1.0
import QtQuick.Window 2.0


Rectangle {
    id: gameView;
    width: 800;
    height: 600;
    visible: true;
    color: "black";
    property string stackName: "gameview";
    property bool run: false;
    property string gameName: "";
    property string coreName: "";
    property bool loadSaveState: false
    property bool saveGameState: false;
    property alias video: videoItem;
    property alias gameMouse: gameMouse;
    property string previousViewIcon: "";

    Component.onCompleted: {
        root.itemInView = "game";
        root.gameShowing = true;
    }
    Component.onDestruction: root.gameShowing = false;

    function timerEffects() {
        if (gameMouse.cursorShape !== Qt.ArrowCursor)
            gameMouse.cursorShape = Qt.ArrowCursor;
        if (headerBar.height === 0) {
            headerBar.height = 50;
        }
        headerBar.timer.restart();
    }

    onSaveGameStateChanged: {
        if (saveGameState) {
            videoItem.saveGameState();
            saveGameState = false;
        }
    }

    onLoadSaveStateChanged: {
        if (loadSaveState) {
            videoItem.loadGameState();
            loadSaveState = false;
        }
    }


    onVisibleChanged: {
        if (visible) {
            ranOnce = true;
            timerEffects();
            headerBar.sliderVisible = false;
            headerBar.searchBarVisible = false;
            prevView = headerBar.viewIcon;
            headerBar.viewIcon = "../assets/GameView/home.png";
        }
        else {
            headerBar.sliderVisible = true;
            headerBar.searchBarVisible = true;
            headerBar.timer.stop();
            if (ranOnce)
                headerBar.viewIcon = prevView;
        }
    }

    MouseArea {
        id: gameMouse;
        anchors.fill: parent;
        hoverEnabled: true
        onMouseXChanged: timerEffects();
        onMouseYChanged: timerEffects();
        onDoubleClicked: {
            root.swapScreenSize();
        }
    }

    VideoItem {
        id: videoItem;
        focus: true;
        anchors {
           centerIn: parent;
        }

        height: parent.height;
        width: stretchVideo ? parent.width : height * aspectRatio;

        systemDirectory: root.systemDirectory;
        saveDirectory: root.saveDirectory;
        libcore: gameView.coreName;
        game: gameView.gameName;
        run: gameView.run;
        volume: root.volumeLevel;
        filtering: root.filtering;
        stretchVideo: root.stretchVideo;

        //property real ratio: width / height;
        //onRatioChanged: console.log(ratio)

        onRunChanged: {
            if (run)
                headerBar.playIcon = "/assets/GameView/pause.png";
            else
                headerBar.playIcon = "/assets/GameView/play.png";
        }


        onSetWindowedChanged: {
            if (root.visibility == Window.FullScreen)
                root.swapScreenSize();
        }

        Component.onDestruction: {
            saveGameState();
        }
    }

    Text {
        id: fpsCounter;
        text: "FPS: " + videoItem.fps;
        color: "#f1f1f1";
        font.pointSize: 16;
        style: Text.Outline;
        styleColor: "black";
        renderType: Text.QtRendering;

         anchors {
            right: parent.right;
            top: parent.top;
            rightMargin: 5;
            topMargin: 5;
        }
    }
}
