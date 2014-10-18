import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1

Item {
    id: core;

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
                    text: "Core Settings"
                    renderType: Text.QtRendering;
                    color: settingsBubble.textColor;
                    font {
                        family: "Sans";
                        pixelSize: 18;
                    }
                }

                Text {
                    text: "Change emulation cores"
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

            RowLayout {
                anchors {
                    centerIn: parent;
                }

                ExclusiveGroup {
                    id: topButtonGroup;
                }

                PhoenixNormalButton {
                    z: preferredButton.z - 1;
                    text: "Consoles";
                    implicitWidth: 70;
                    checkable: true;
                    onClicked: {
                        if (coreSettingsStack.currentItem.stackName !== "coreConsoles") {
                            settingsWindow.height = 400;
                            settingsWindow.width = 250;
                            coreSettingsStack.push({item: coreConsoles, replace: true, immediate: true})
                        }
                    }
                    exclusiveGroup: topButtonGroup;
                    anchors {
                        right: preferredButton.left;
                        rightMargin: -1;
                    }
                }
                PhoenixNormalButton {
                    id: preferredButton;
                    text: "Preferred";
                    implicitWidth: 70;
                    radius: 0;
                    exclusiveGroup: topButtonGroup;
                    checkable: true;onClicked: {
                        if (coreSettingsStack.currentItem.stackName !== "corePreferred") {
                            settingsWindow.height = 550;
                            settingsWindow.width = 300;
                            coreSettingsStack.push({item: corePreferred, replace: true, immediate: true})
                        }
                    }
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                    }

                }

                PhoenixNormalButton {
                    z: preferredButton.z - 1;
                    text: "Information";
                    implicitWidth: 70;
                    exclusiveGroup: topButtonGroup;
                    checkable: true;
                    onClicked: {
                        if (coreSettingsStack.currentItem.stackName !== "coreInformation")
                            coreSettingsStack.push({item: coreInformation, replace: true, immediate: true})
                    }
                    anchors {
                        left: preferredButton.right;
                        leftMargin: -1;
                    }
                }
            }
        }
    }

    StackView {
        id: coreSettingsStack;

        initialItem: coreConsoles;
    }

    Component {
        id: corePreferred;
        Item {
            id: video;

            property string stackName: "corePreferred";

            PhoenixScrollView {
                frameColor: "#000000FF";
                borderEnabled: false;
                handleHeight: 10;
                anchors {
                    fill: parent;
                    topMargin: 15;
                    //rightMargin: 15;
                    leftMargin: 15;
                }
                ListView {
                    spacing: 5;
                    model: phoenixLibrary.systemsModel();
                    delegate: RowLayout {
                        visible: modelData !== "All";
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        spacing: 25;
                        Text {
                            text: modelData;
                            renderType: Text.QtRendering;
                            color: "#f1f1f1";
                            font {
                                family: "Sans";
                                pixelSize: 12;
                            }
                        }

                        PhoenixComboBox {
                            id: filteringSwitch;
                            implicitHeight: 25;
                            anchors {
                                right: parent.right;
                                rightMargin: 25;
                            }
                            model: phoenixLibrary.coresModel(modelData);
                            textRole: "coreName";
                            onCurrentIndexChanged:  {
                                var obj = phoenixLibrary.showPath(currentIndex, modelData);
                                phoenixLibrary.setPreferredCore(modelData, obj);
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: coreInformation;

        TableView {
            id: table;

            property string itemName: "table";
            property string highlightColor: "#4a4a4a";
            property string textColor: "#f1f1f1";
            property string headerColor: "#262626";

            property string stackName: "coreInformation";
            model: ListModel {
                ListElement {title: "Version"}
            }

            TableViewColumn {role: "title"; title: "Version"; width: 100;}

            ExclusiveGroup {
                id: rowGroup;
            }

            headerDelegate: Rectangle {
                color: table.headerColor;
                height: 25;
                width: parent.width;

                Rectangle {
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.right;
                    }
                    height: 1;

                    color: "#303030";
                }

                Row {
                    id: rightRowBorder;
                    anchors {
                        right: parent.right;
                        top: parent.top;
                        bottom: parent.bottom;
                    }

                    Rectangle {
                        color: "#1a1a1a";
                        anchors {
                            top: parent.top;
                            bottom: parent.bottom;
                        }
                        width: 1;
                    }
                }

                Column {
                    anchors {
                        right: rightRowBorder.left;
                        left: parent.left;
                        bottom: parent.bottom;
                    }

                    Rectangle {
                        anchors {
                            right: parent.right;
                            left: parent.left;
                        }
                        color: "#1a1a1a";
                        height: 1;
                    }
                }



                Text {
                    text: styleData.value;
                    renderType: Text.QtRendering;
                    anchors {
                        left: parent.left;
                        leftMargin: 15;
                        verticalCenter: parent.verticalCenter;
                    }
                    color: table.textColor;

                    font {
                        bold: true;
                        pixelSize: 11;
                        family: "Sans";
                    }

                }

            }

            style: TableViewStyle {
                id: tableStyle;

                frame: Rectangle {
                    color: "#202020";
                    width: 1;
                }
                property int handleWidth: 10
                /*frame: Rectangle {
                    color: gameGrid.color;
                    //width: 0;
                    Rectangle {
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#1a1a1a";
                    }
                }*/

                transientScrollBars: true;
                scrollToClickedPosition: true;
                handleOverlap: -3;

                handle: Rectangle {
                    color: "red"
                    radius: 3;
                    x: 3;
                    implicitWidth: tableStyle.handleWidth - (x * 2);
                    implicitHeight: 30;
                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "#2b2a2b";}
                        GradientStop {position: 1.0; color: "#252525";}
                    }

                    Rectangle {
                        color: "#525254";
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            leftMargin: 1;
                            rightMargin: 1;
                            right: parent.right;
                        }
                        height: 1;
                    }

                    Rectangle {
                        anchors {
                            left: parent.left;
                            top: parent.top;
                            bottom: parent.bottom;
                            bottomMargin: 1;
                            topMargin: 1;
                        }
                        color: "#414142";
                        width: 1;
                    }

                    Rectangle {
                        anchors {
                            right: parent.right;
                            top: parent.top;
                            bottom: parent.bottom;
                            bottomMargin: 1;
                            topMargin: 1;
                        }
                        color: "#414142";
                        width: 1;
                    }

                    Rectangle {
                        color: "#323233";
                        anchors {
                            bottom: parent.bottom;
                            left: parent.left;
                            leftMargin: 1;
                            rightMargin: 1;
                            right: parent.right;
                        }
                        height: 1;
                    }
                }

                scrollBarBackground: Rectangle {
                    radius: 3;
                    color: "#1c1c1c";
                    width: styleData.hovered ? 16 : 12;
                    onWidthChanged:  {
                        if (styleData.hovered)
                            tableStyle.handleWidth = 16;
                        else
                            tableStyle.handleWidth = 12;
                    }

                    border {
                        width: 1;
                        color: "#1a1a1a";
                    }
                }

                rowDelegate: Rectangle {
                    id: row;
                    color: styleData.alternate ? "#262626" : "#2e2e2e";
                    width: parent.width;
                    height: 25;

                    property ExclusiveGroup exclusiveGroup: rowGroup;
                    property bool checked: false

                    onExclusiveGroupChanged: {
                        if (exclusiveGroup) {
                            exclusiveGroup.bindCheckable(row);
                        }
                    }

                    onCheckedChanged: {
                        if (checked) {
                            color = table.highlightColor;
                        }
                        else {
                            color = styleData.alternate ? "#262626" : "#2e2e2e";
                        }
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onPressed: row.checked = true;
                    }

                }

                itemDelegate: Item {
                    Text {
                        anchors {
                            left: parent.left;
                            leftMargin: 15;
                            verticalCenter: parent.verticalCenter;
                        }
                        renderType: Text.QtRendering;
                        width: control.width;
                        color: "#f1f1f1";
                        text: styleData.value;

                        font {
                            pixelSize: 11;
                            family: "Sans";
                        }

                        elide: Text.ElideRight;

                    }
                }
            }
        }

    }

    Component {
        id: coreConsoles;
        Item {
            property string  stackName: "coreConsoles";

            Column {
                spacing: 15;
                anchors {
                    //fill: parent;
                    //topMargin: 15;
                    left: parent.left;
                    right: parent.right;
                    rightMargin: 15;
                    leftMargin: 15;
                }

                Repeater {
                    anchors.left: parent.left;
                    model: ["Nintendo", "Super Nintendo", "Game Boy", "Game Boy Advance", "Sony PlayStation"];
                    delegate: CheckBox {
                        anchors {
                            left: parent.left;
                            leftMargin: 15;
                        }

                        text: modelData;
                        style: CheckBoxStyle {
                            spacing: 15;
                            indicator: Rectangle {
                                radius: 3;
                                height: 15;
                                width: 15;
                                color: "#232323";
                                border {
                                    width: 1;
                                    color: control.checked ? "#3fe86b" : "#e8433f";
                                }

                                Rectangle {
                                    visible: control.checked ? true : false;
                                    anchors {
                                        fill: parent;
                                        margins: 5;
                                    }

                                    radius: 10;
                                    color: "#f1f1f1";
                                }
                            }

                            label: Text {
                                renderType: Text.QtRendering;
                                font {
                                    family: "Sans";
                                    pixelSize: 13;
                                }
                                //horizontalAlignment: Text.AlignRight;
                                text: control.text;
                                color: control.checked ? settingsBubble.textColor : settingsBubble.alternateTextColor;
                            }
                        }
                    }
                }
            }
        }
    }
}
}
