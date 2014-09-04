import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import Qt.labs.settings 1.0
import phoenix.video 1.0


Item {
    id: gameView;
    width: 800;
    height: 600;
    visible: true;

    property string stackName: "gameview";

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
    property int filtering: 2;
    property bool stretchVideo: false;

    onVolumeLevelChanged: {
        console.log("volume level: " + volumeLevel);
    }

    property alias video: videoItem;

    Settings {

        category: "Video";
        //property alias vsync: gameView.checked;
        property alias smooth: gameView.filtering;
        property alias stretchVideo: gameView.stretchVideo;
        property alias volumeLevel: gameView.volumeLevel;
        //property alias autoFullscreen: autoFullscreenSwitch.checked;
    }


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
        anchors.fill: parent;
        systemDirectory: gameView.systemDirectory;
        saveDirectory: gameView.saveDirectory;
        libcore: gameView.coreName;
        game: gameView.gameName;
        run: gameView.run;
        volume: gameView.volumeLevel;
        filtering: gameView.filtering;
        stretchVideo: gameView.stretchVideo;

        onRunChanged: {
            if (run)
                headerBar.playIcon = "/assets/GameView/pause.png";
            else
                headerBar.playIcon = "/assets/GameView/play.png";
        }


        onSetWindowedChanged: {
            if (root.visibility == 5)
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
