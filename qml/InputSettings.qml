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
                    model: ListModel {
                        id: buttonsModel;
                        ListElement {controllerButton: "Up"; retroId: "4"; updating: false;}
                        ListElement {controllerButton: "Down"; retroId: "5"; updating: false;}
                        ListElement {controllerButton: "Left"; retroId: "6"; updating: false;}
                        ListElement {controllerButton: "Right"; retroId: "7"; updating: false;}
                        ListElement {controllerButton: "Select"; retroId: "2"; updating: false;}
                        ListElement {controllerButton: "Start"; retroId: "3"; updating: false;}
                        ListElement {controllerButton: "A"; retroId: "8"; updating: false;}
                        ListElement {controllerButton: "B"; retroId: "0"; updating: false;}
                        ListElement {controllerButton: "X"; retroId: "9"; updating: false;}
                        ListElement {controllerButton: "Y"; retroId: "1"; updating: false;}
                        ListElement {controllerButton: "R"; retroId: "11"; updating: false;}
                        ListElement {controllerButton: "L"; retroId: "10"; updating: false;}
                        ListElement {controllerButton: "RB"; retroId: "13"; updating: false;}
                        ListElement {controllerButton: "LB"; retroId: "12"; updating: false;}
                        ListElement {controllerButton: "R3"; retroId: "15"; updating: false;}
                        ListElement {controllerButton: "L3"; retroId: "14"; updating: false;}
                    }

                    delegate: Item {
                        id: gridItem;
                        height: 30;
                        width: 225;
                        property bool overrideFocus: false;

                        function keyReceived(ev, value) {
                            if (value) {
                                var prevBinding = inputMapper.mapping.getMappingByRetroId(retroId);
                                console.log("RECEIVED event: " + ev + " and value: " + value);
                                inputMapper.mapping.remapMapping(prevBinding, ev, retroId, inputMapper.deviceIndex);
                                inputMapper.waitingUpdate = false;
                                buttonsModel.get(index).updating = false;
                                console.log("New binding: " + inputMapper.mapping.getMappingByRetroId(retroId));
                                inputMapper.device.inputEventReceived.disconnect(keyReceived);

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
                            buttonsModel.get(index).updating = true;
                            console.log("Changing mapping for " + controllerButton + " on device: " + inputMapper.device.deviceName());
                            inputMapper.device.inputEventReceived.connect(gridItem.keyReceived);
                        }



                        onOverrideFocusChanged: {
                            if (overrideFocus) {
                                startUpdate();
                            }
                        }

                        Text {
                            renderType: Text.QtRendering;
                            text: controllerButton + ":";
                            anchors {
                                right: buttonField.left;
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
                            id: buttonField;
                            readOnly: true;
                            width: 100;
                            height: 25;
                            radius: 3;
                            borderColor: "black";
                            renderType: Text.QtRendering;
                            textColor: "#f1f1f1";
                            text: updating ? "WAITING" : inputMapper.mapping.getMappingByRetroId(retroId) ;
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
