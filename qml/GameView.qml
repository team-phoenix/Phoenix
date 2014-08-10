import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

import VideoItem 1.0


Item {
    id: gameView;
    width: 800;
    height: 600;
    visible: true;
    property bool run: false;
    property string gameName: "";
    property string coreName: "";
    property string systemDirectory: "";
    property string saveDirectory: "";
    property bool loadSaveState: false
    property bool saveGameState: false;
    property real volumeLevel: 1.0;
    property string prevView: "";
    property bool ranOnce: false;
    property bool screenTimer: false;
    property alias gameMouse: gameMouse;

    function timerEffects() {
        if (gameMouse.cursorShape !== Qt.ArrowCursor)
            gameMouse.cursorShape = Qt.ArrowCursor;
        if (headerBar.height === 0) {
            headerBar.height = 50;
        }
        headerBar.timer.restart();
    }

    onSaveGameStateChanged: {
        if (saveGameState)
            videoItem.saveGameState();
    }

    onLoadSaveStateChanged: {
        if (loadSaveState)
            videoItem.loadGameState();
    }


    onVisibleChanged: {
        if (visible) {
            ranOnce = true;
            timerEffects();
            headerBar.opacity = 0.75;
            headerBar.sliderVisible = false;
            headerBar.searchBarVisible = false;
            prevView = headerBar.viewIcon;
            headerBar.viewIcon = "../assets/GameView/home.png";
        }
        else {
            headerBar.opacity = 1.0;
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
            if (visibility == 5)
                visibility = "Windowed";
            else
                visibility = "FullScreen";
        }
    }

    VideoItem {
        id: videoItem;
        focus: true;
        anchors.fill: parent;
        systemDirectory: gameView.systemDirectory;
        saveDirectory: gameView.saveDirectory;
        libcore: gameView.coreName;
        game: gameView.gameName;
        run: gameView.run;
        volume: gameView.volumeLevel;
        onRunChanged: {
            if (run)
                headerBar.playIcon = "/assets/GameView/pause.png";
            else
                headerBar.playIcon = "/assets/GameView/play.png";
        }

        onSetWindowedChanged: {
            if (visibility != 2)
                visibility = "Windowed";
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

         anchors {
            right: parent.right;
            top: parent.top;
            rightMargin: 5;
            topMargin: 5;
        }
    }
}
