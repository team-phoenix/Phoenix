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
                visible: stackView.width > width;
                height: 400;
                width: 400;
                GridView {
                    //anchors.fill: parent;
                    height: parent.height;
                    width: parent.width;
                    cellHeight: 30;
                    cellWidth: 150;
                    header: Item {
                        height: 45;
                        width: 125;

                        Row {
                            anchors {
                                fill: parent;
                                leftMargin: 60;
                            }
                            spacing: 15;

                            ComboBox {
                                property int num: devicesBox.count;
                                width: 125;
                                model:  {
                                    var mod = [];
                                    for (var i=0; i < num; ++i) {
                                        mod.push("Player " + (i + 1));
                                    }
                                    return mod;
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
                        ListElement {controllerButton: "Up"; retroId: "0";}
                        ListElement {controllerButton: "Down"; retroId: "1";}
                        ListElement {controllerButton: "Left"; retroId: "2";}
                        ListElement {controllerButton: "Right"; retroId: "3";}
                        ListElement {controllerButton: "Select"; retroId: "4";}
                        ListElement {controllerButton: "Start"; retroId: "5";}
                        ListElement {controllerButton: "A"; retroId: "6";}
                        ListElement {controllerButton: "B"; retroId: "7";}
                        ListElement {controllerButton: "X"; retroId: "8";}
                        ListElement {controllerButton: "Y"; retroId: "9";}
                        ListElement {controllerButton: "R"; retroId: "10";}
                        ListElement {controllerButton: "L"; retroId: "11";}
                        ListElement {controllerButton: "RB"; retroId: "A1";}
                        ListElement {controllerButton: "LB"; retroId: "A2";}
                        ListElement {controllerButton: "R3"; retroId: "12";}
                        ListElement {controllerButton: "L3"; retroId: "13";}

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
                            text: retroId;
                            anchors.right: parent.right;
                            anchors.rightMargin: 50;
                            horizontalAlignment: Text.AlignHCenter;
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: {
                                    //inputmanager.iterateDevices();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
