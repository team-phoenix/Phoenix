import QtQuick 2.3
import QtQuick.Controls 1.1

Item {

    Column {
        anchors {
            fill: parent;
            topMargin: 25;
        }
        spacing: 25;

        Column {
            id: inputHeader;
            anchors {
                left: parent.left;
                //top: parent.top;
                //topMargin: 25;
                leftMargin: 25;
            }
            spacing: 5;

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
                text: "Adjust your input devices"
                renderType: Text.QtRendering;
                color: settingsBubble.alternateTextColor;
                font {
                    family: "Sans";
                    pixelSize: 14;
                }
            }
        }

        Row {
            anchors {
                left: parent.left;
                leftMargin: 25;
                right: parent.right;
            }
            height: 500;
            spacing: 15;

            Image {
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

            }



            ScrollView {
                id: inputMapper;
                property var mapping: gridView.headerItem.curMapping;
                property var device: gridView.headerItem.curDevice;
                visible: stackView.width > width;
                property bool waitingUpdate: false;
                height: 400;
                width: 400;
                GridView {
                    id: gridView;
                    //anchors.fill: parent;
                    height: parent.height;
                    width: parent.width;
                    cellHeight: 30;
                    cellWidth: 150;
                    header: Item {
                        property alias curMapping: playersBox.curMapping;
                        property alias curDevice: playersBox.curDevice;
                        height: 45;
                        width: 125;

                        Row {
                            anchors {
                                fill: parent;
                                leftMargin: 60;
                            }
                            spacing: 15;

                            ComboBox {
                                id: playersBox;
                                property int num: devicesBox.count;
                                width: 125;
                                property var curMapping;
                                property var curDevice;
                                model:  {
                                    var mod = [];
                                    for (var i=0; i < num; ++i) {
                                        mod.push("Player " + (i + 1));
                                    }
                                    return mod;
                                }
                                onCurrentIndexChanged: {
                                    curMapping = inputmanager.mappingForPort(currentIndex);
                                    curDevice = inputmanager.getDevice(currentIndex);
                                }
                            }
                            ComboBox {
                                id: devicesBox;
                                width: 125;
                                model: inputmanager.enumerateDevices();
                            }
                        }
                    }

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
                        height: 30;
                        width: 225;
                        Text {
                            renderType: Text.QtRendering;
                            text: controllerButton;
                            anchors {
                                right: buttonField.left;
                                rightMargin: 15;
                            }

                            color: settingsBubble.textColor;
                            font {
                                pixelSize: 12;
                            }
                        }

                        TextField {
                            id: buttonField;
                            readOnly: true;
                            width: 100;
                            height: 20;
                            text: updating ? "WAITING" : inputMapper.mapping.getMappingByRetroId(retroId);
                            anchors.right: parent.right;
                            anchors.rightMargin: 50;
                            horizontalAlignment: Text.AlignHCenter;
                            MouseArea {
                                id: buttonMouseArea;
                                anchors.fill: parent;
                                onClicked: {
                                    if (inputMapper.waitingUpdate)
                                        return;
                                    inputMapper.waitingUpdate = true;
                                    buttonsModel.get(index).updating = true;
                                    console.log("Changing mapping for " + controllerButton + " on device: " + inputMapper.device.deviceName());
                                    inputMapper.device.inputEventReceived.connect(keyReceived);
                                }

                                function keyReceived(ev, value) {
                                    var prevBinding = inputMapper.mapping.getMappingByRetroId(retroId);
                                    console.log("RECEIVED event: " + ev + " and value: " + value);
                                    inputMapper.mapping.remapMapping(prevBinding, ev, retroId);
                                    inputMapper.waitingUpdate = false;
                                    buttonsModel.get(index).updating = false;
                                    console.log("New binding: " + inputMapper.mapping.getMappingByRetroId(retroId));
                                    inputMapper.device.inputEventReceived.disconnect(keyReceived);
                                }

                                Connections {
                                    target: settingsWindow;

                                    onVisibleChanged: {
                                        if (!settingsWindow.visible) {
                                            inputMapper.waitingUpdate = false;
                                            inputMapper.device.inputEventReceived.disconnect(buttonMouseArea.keyReceived);
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
