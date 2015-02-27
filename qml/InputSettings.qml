import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Item {
    id: inputSettings;
    property var currentMapping;
    property var currentDevice;
    property int currentPort: 0;

    Column {
        anchors {
            fill: parent;
            topMargin: 15;
        }
        spacing: 30;

        Item {
            height: 75;
            anchors {
                left: parent.left;
                right: parent.right;
            }

            Row {
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                }
                spacing: 10;

                Image {
                    id: videoImage;
                    source: "../assets/Core-64.png";
                    height: 48;
                    width: 48;
                }

                Column {
                    id: videoHeader;
                    anchors.verticalCenter: videoImage.verticalCenter;
                    spacing: 2;

                    Text {
                        text: "Input Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Configure Game Controllers"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 13;
                        }
                    }
                }
            }

            Rectangle {
                color: "#141414";
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                    bottomMargin: 0;
                    leftMargin: 5;
                    rightMargin: 5;
                }
                height: 1;

                ExclusiveGroup {
                    id: topButtonGroup;
                }
                ListView {
                    id: listView;

                    z: 1;
                    anchors.centerIn: parent;
                    orientation: ListView.Horizontal;
                    width: (spacing !== 0) ? currentItem.width * count * spacing: currentItem.width * count;
                    height: currentItem.height;
                    interactive: false;

                    Component.onCompleted: updateDevices();

                    model: inputmanager.enumerateDevices();
                    /* ListModel {
                        ListElement {player: "Player 1"; selected: true; curvature: 3; port: 0}
                        ListElement {player: "Player 2"; selected: false; curvature: 0; port: 1}
                        ListElement {player: "Player 3"; selected: false; curvature: 0; port: 2}
                        ListElement {player: "Player 4"; selected: false; curvature: 3; port: 3}
                    }*/

                    function updateDevices()
                    {
                        inputSettings.currentDevice = inputmanager.getDevice(currentIndex);
                        inputSettings.currentMapping = inputSettings.currentDevice.mapping();
                        inputSettings.currentPort = currentIndex;
                    }

                    currentIndex: 0;

                    onCurrentIndexChanged: {
                        if (inputmanager.count > 0 && currentIndex < inputmanager.count) {
                            if (currentIndex > 0 && inputSettings.currentDevice !== undefined)
                                inputSettings.currentDevice.inputEventReceived.disconnect(gridView.currentItem.keyReceived);
                            listView.updateDevices();
                        }
                    }



                    Connections {
                        target: inputmanager;
                        onCountChanged: {
                            inputmanager.updateModel();
                            listView.updateDevices();
                        }
                    }

                    delegate: PhoenixNormalButton {
                        property bool innerItem: false;
                        property string type: listView.model[index]["type"];
                        text: type == "GamePad" ? "GamePad " + index : type;
                        z: innerItem ? listView.z + 1 : listView.z;
                        radius: 0;
                        implicitWidth: 70;
                        checkable: true;
                        checked: ListView.isCurrentItem;
                        exclusiveGroup: topButtonGroup;

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: listView.currentIndex = index;
                        }

                    }
                }
            }
        }

        Row {
            z: 100;

            anchors {
                horizontalCenter: parent.horizontalCenter;
            }

            spacing: 15;

            PhoenixNormalButton {
                text: "Configure All";
                implicitWidth: 100;
                onClicked: {
                    if (inputmanager.count > 0) {
                        gridView.currentIndex = 0;
                        inputMapper.walkthroughCount = 0;
                        inputMapper.setupWalkthrough = false;
                        inputMapper.setupWalkthrough = true;
                    }
                }
            }
        }

        Row {
            anchors {
                horizontalCenter: parent.horizontalCenter;
            }
            spacing: 15;

           /*Image {
                visible: stackView.width > width;

                fillMode: Image.PreserveAspectFit;
                height: 250;
                width: 250;
                sourceSize {
                    height: 500;
                    width: 500;
                }
                source: "../assets/retropad.png";
                anchors.verticalCenter: parent.verticalCenter;
                anchors.verticalCenterOffset: -110;

            }*/



            ScrollView {
                id: inputMapper;

                //visible: stackView.width > width;
                property bool waitingUpdate: false;
                property bool setupWalkthrough: false;
                property int walkthroughCount: 0;

                onSetupWalkthroughChanged: {
                    gridView.currentItem.overrideFocus = true;
                }
                Component.onDestruction: waitingUpdate = false;
                height: 350;
                width: 400;

                GridView {
                    id: gridView;
                    //anchors.fill: parent;
                    height: parent.height;
                    width: parent.width;
                    cellHeight: 40;
                    cellWidth: 175;
                    flow: GridView.TopToBottom;
                    model: ListModel {
                        ListElement {retroId: 8; button: "A";}
                        ListElement {retroId: 0; button: "B";}
                        ListElement {retroId: 9; button: "X";}
                        ListElement {retroId: 1; button: "Y";}
                        ListElement {retroId: 2; button: "Select";}
                        ListElement {retroId: 3; button: "Start";}
                        ListElement {retroId: 4; button: "Up";}
                        ListElement {retroId: 5; button: "Down";}
                        ListElement {retroId: 6; button: "Left";}
                        ListElement {retroId: 7; button: "Right";}
                        ListElement {retroId: 10; button: "L";}
                        ListElement {retroId: 11; button: "R";}
                        ListElement {retroId: 12; button: "L2";}
                        ListElement {retroId: 13; button: "R2";}
                        ListElement {retroId: 14; button: "L3";}
                        ListElement {retroId: 15; button: "R3";}
                    }

                    Timer {
                        id: textFieldTimer;
                        interval: 500;
                        onTriggered: gridView.currentItem.buttonField.state = "waiting";
                    }

                    function nullTest(data)
                    {
                        return data === null;
                    }


                    delegate: Item {
                        id: gridItem;
                        height: 30;
                        width: 225;
                        property bool overrideFocus: false;
                        property alias buttonField: textField;
                        property string event: inputSettings.currentMapping.getMappingByRetroId(retroId);

                        function keyReceived(ev, value) {
                            if (value) {
                                var event = inputmanager.variantToString(ev);
                                if (!inputSettings.currentMapping.remap(ev, retroId, inputSettings.currentPort)) {
                                    textField.state = "collision";
                                    textFieldTimer.start();
                                    return;
                                }

                                inputMapper.waitingUpdate = false;
                                inputSettings.currentDevice.inputEventReceived.disconnect(keyReceived);

                                gridItem.event = event;
                                textField.state = "normal";

                                if (inputMapper.setupWalkthrough) {
                                    inputMapper.walkthroughCount += 1;
                                    gridItem.overrideFocus = false;
                                    if (inputMapper.walkthroughCount < gridView.count) {
                                        gridView.currentIndex += 1;
                                        gridView.currentItem.overrideFocus = true;
                                    }
                                }
                            }
                        }

                        function startUpdate() {
                            if (inputMapper.waitingUpdate)
                                return;

                            inputMapper.waitingUpdate = true;

                            textField.state = "waiting";


                            console.log("Changing mapping for " + gridItem.event + " on device: " + inputSettings.currentDevice.deviceName());
                            inputSettings.currentDevice.inputEventReceived.connect(gridItem.keyReceived);
                        }

                        onOverrideFocusChanged: {
                            if (overrideFocus) {
                                startUpdate();
                            }
                        }

                        Text {
                            renderType: Text.QtRendering;
                            text: button;
                            anchors {
                                right: textField.left;
                                verticalCenter: parent.verticalCenter;
                                rightMargin: 15;
                            }

                            color: settingsBubble.textColor;
                            font {
                                pixelSize: 11;
                                bold: true;
                                family: "Sans";
                            }

                        }

                        PhoenixTextField {
                            id: textField;
                            readOnly: true;
                            width: 100;
                            height: 25;
                            radius: 3;
                            renderType: Text.QtRendering;
                            textColor: "#f1f1f1";
                            text: gridItem.event;
                            horizontalAlignment: Text.AlignHCenter;

                            anchors {
                                verticalCenter: parent.verticalCenter;
                                right: parent.right;
                                rightMargin: 50;
                            }

                            font {
                                family: "Sans";
                                pixelSize: 11;
                            }

                            Component.onCompleted: state = "normal";

                            states: [
                                State {
                                    name: "collision";
                                    PropertyChanges {
                                        target: textField;
                                        borderWidth: 4;
                                        borderColor: "red";
                                    }
                                },

                                State {
                                    name: "waiting";
                                    PropertyChanges {
                                        target: textField;
                                        borderWidth: 4;
                                        borderColor: "blue";
                                    }
                                },

                                State {
                                    name: "normal";
                                    PropertyChanges {
                                        target: textField;
                                        borderWidth: 1;
                                        borderColor: "black";
                                    }

                                }

                            ]

                            transitions: [
                                Transition {
                                    from: "normal";
                                    to: "waiting";

                                        NumberAnimation {
                                            properties: "borderWidth";
                                            easing.type: Easing.InBack;
                                            duration: 200;
                                        }

                                },
                                Transition {
                                    from: "waiting";
                                    to: "normal";
                                        NumberAnimation {
                                            properties: "borderWidth";
                                            easing.type: Easing.Linear;
                                            duration: 10;
                                        }
                                }

                            ]

                            MouseArea {
                                id: buttonMouseArea;
                                anchors.fill: parent;
                                onClicked: {
                                    if (textFieldTimer.running)
                                        textFieldTimer.stop();
                                    console.log("shoudl update");
                                    gridView.currentIndex = index;
                                    gridItem.startUpdate();
                                }
                                    //inputSettings.currentDevice.inputEventReceived.disconnect(buttonMouseArea.keyReceived);
                            }
                        }
                    }
                }
            }
        }
    }
}
