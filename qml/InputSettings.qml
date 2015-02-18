import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.2

Item {

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

                    property var curMapping;
                    property var curDevice;

                    z: 1;
                    anchors.centerIn: parent;
                    orientation: ListView.Horizontal;
                    width: (spacing !== 0) ? currentItem.width * count * spacing: currentItem.width * count;
                    height: currentItem.height;

                    model: ListModel {
                        ListElement {player: "Player 1"; selected: true; curvature: 3;}
                        ListElement {player: "Player 2"; selected: false; curvature: 0;}
                        ListElement {player: "Player 3"; selected: false; curvature: 0;}
                        ListElement {player: "Player 4"; selected: false; curvature: 3;}
                    }
                    onCurrentIndexChanged: {
                        inputMapper.deviceIndex = currentIndex;
                        curMapping = inputmanager.mappingForPort(currentIndex);
                        curDevice = inputmanager.getDevice(currentIndex);
                    }
                    delegate: PhoenixNormalButton {
                        property bool innerItem: (curvature == 0);

                        text: player;
                        z: innerItem ? listView.z + 1 : listView.z;
                        radius: 0;
                        implicitWidth: 70;
                        checkable: true;
                        checked: selected;
                        exclusiveGroup: topButtonGroup;


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

            ComboBox {
                id: devicesBox;
                width: 125;
                model: inputmanager.enumerateDevices();
                onCurrentIndexChanged: {
                    //listView.curDevice = inputmanager.getDevice(currentIndex);
                }

            }

            PhoenixNormalButton {
                text: "Configure All";
                implicitWidth: 100;
                onClicked: {
                    gridView.currentIndex = 0;
                    inputMapper.walkthroughCount = 0;
                    inputMapper.setupWalkthrough = false;
                    inputMapper.setupWalkthrough = true;
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
                property var mapping: listView.curMapping;
                property alias device: listView.curDevice;
                property int deviceIndex: 0;
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

                    Component.onCompleted: {
                        inputMapper.mapping.updateModel();
                        model = inputMapper.mapping.model();
                    }

                    Timer {
                        id: textFieldTimer;
                        interval: 500;
                        onTriggered: {
                            //gridView.currentItem.buttonField.text = gridView.currentItem.textFieldText;
                            gridView.currentItem.buttonField.state = "normal";
                        }
                    }


                    delegate: Item {
                        id: gridItem;
                        height: 30;
                        width: 225;
                        property bool overrideFocus: false;
                        property alias buttonField: textField;




                        function keyReceived(ev, value) {
                            if (value) {
                                var event = inputMapper.mapping.variantToString(ev);
                                if (inputMapper.mapping.collisionDetected(event, index)) {
                                    textField.state = "collision";
                                    //textField.text = "'" + event + "'" + " is already used.";
                                    textFieldTimer.start();
                                    //inputMapper.device.inputEventReceived.disconnect(keyReceived);
                                    return;
                                }

                                //var prevBinding = modelData.deviceEvent//inputMapper.mapping.getMappingByRetroId(modelData.d);
                                //console.log("RECEIVED event: " + ev + " and value: " + value);
                                //inputMapper.mapping.remapMapping(prevBinding, ev, modelData.retroID, inputMapper.deviceIndex);
                                inputMapper.waitingUpdate = false;
                                modelData.updating = false;
                                modelData.deviceEvent = event;

                                inputMapper.device.inputEventReceived.disconnect(keyReceived);
                                inputMapper.mapping.saveModel(inputMapper.deviceIndex);
                                //textField.text = modelData.updating ? "Waiting..." : modelData.deviceEvent;
                                console.log(modelData.deviceEvent + " was set! ");

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

                            modelData.updating = true;
                            console.log("Changing mapping for " + modelData.deviceEvent + " on device: " + inputMapper.device.deviceName());
                            inputMapper.device.inputEventReceived.connect(gridItem.keyReceived);
                        }



                        onOverrideFocusChanged: {
                            if (overrideFocus) {
                                startUpdate();
                            }
                        }

                        Text {
                            renderType: Text.QtRendering;
                            text: inputMapper.mapping.getGamepadName(modelData.retroID) + ":";
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

                            text: modelData.updating ? "Waiting..." : modelData.deviceEvent;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                right: parent.right;
                                rightMargin: 50;
                            }

                            horizontalAlignment: Text.AlignHCenter;
                            font {
                                family: "Sans";
                                pixelSize: 11;
                            }

                            MouseArea {
                                id: buttonMouseArea;
                                anchors.fill: parent;
                                onClicked: {
                                    if (textFieldTimer.running)
                                        textFieldTimer.stop();

                                    gridView.currentIndex = index;
                                    gridItem.startUpdate();
                                }
                                    //inputMapper.device.inputEventReceived.disconnect(buttonMouseArea.keyReceived);
                            }
                        }
                    }
                }
            }
        }
    }
}
