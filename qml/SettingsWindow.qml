import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

Item {
    id: settingsBubble;
    anchors.fill: parent;
    property string groupingColor: "gray";
    property string stackBackgroundColor: "darkgray";
    property string contentColor: "#d7d7d7";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#898989";
    property bool expand: false;

    property bool contentVisible: false;
    width: 500;

    Rectangle {
        id: menuBar;
        z: stackView.z + 1;
        color: settingsBubble.stackBackgroundColor;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
        }
        width: 150;

        ExclusiveGroup {
            id: menuGroup;
            property string buttonBackgroundColor: "#000000FF";
        }

        Rectangle {
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
                topMargin: 20;
                bottomMargin: 20;
            }
            width: 1;
            color: "#e4e4e4";
        }

        ListView {
            id: listView;
            anchors {
                top: parent.top;
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: 15;
                //bottomMargin: 50;
            }
            highlightFollowsCurrentItem: false;
            header: Item {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    leftMargin: 30;
                }
                height: 20;
                Label {
                    //anchors.centerIn: parent;
                    renderType: Text.QtRendering;
                    color: "#f1f1f1";
                    text: "General";
                    font {
                        family: "Sans"
                        pixelSize: 14;
                        bold: true;
                    }
                    horizontalAlignment: Text.AlignHCenter;
                    //verticalAlignment: Text.AlignVCenter;
                }
            }

            highlight: Item {
                id: highlightItem;
                height: listView.currentItem.height;
                width: listView.width;
                anchors.verticalCenter: listView.currentItem.verticalCenter;
                y: listView.currentItem.y;
                Item {
                    id: innerItem;
                    height: parent.height;
                    width: parent.width;

                    Rectangle {
                        id: topBorder;
                        z: mainColor.z + 1;
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#ababab";
                    }

                    Rectangle {
                        id: mainColor;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top: parent.top;
                            bottom: parent.bottom;
                        }
                        color: listView.currentItem ? "#e4e4e4" : "#000000FF";
                    }

                    Rectangle {
                        z: topBorder.z;
                        anchors {
                            bottom: parent.bottom;
                            left: parent.left;
                            right: parent.right;
                        }
                        height: 1;
                        color: "#d0d0d0";
                    }

                }
            }

            property var stacks: { "Input": inputSettings,
                                   "Cores": coreSettings,
                                   "Info": infoSettings,
                                   "Library": librarySettings,
                                   "Save": saveSettings,
                                   "Video": videoSettings,
                                   "Audio": audioSettings,
                                   "Advanced": advancedSettings};
            property string currentName: "";
            model: ListModel {
                ListElement {title: "Input";}
                ListElement {title: "Library";}
                ListElement {title: "Save";}
                ListElement {title: "Cores";}
                ListElement {title: "Advanced";}
                ListElement {title: "Video";}
                ListElement {title: "Audio";}

            }

            delegate: Label {
                height: 25;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    leftMargin: 45;
                }
                font {
                    family: "Sans";
                    bold: true;
                    pixelSize: 13;
                }

                color: settingsBubble.textColor;
                text: title;
                horizontalAlignment: Text.AlignLeft;
                verticalAlignment: Text.AlignVCenter;

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        if (title === listView.currentName) {
                            settingsBubble.expand = false;
                            listView.currentName = "";
                        }
                        else {
                            listView.currentIndex = index;
                            settingsBubble.expand = true;

                            stackView.push({item: listView.stacks[title], replace: true, immediate: true});
                            listView.currentName = title;
                        }
                    }
                }
            }
        }

        /*Rectangle {
            id: advancedRectangle;
            z: listView.z + 1;
            height: 75;
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }

            color: "#242323";

        }

        DropShadow {
            source: advancedRectangle;
            anchors.fill: source;
            horizontalOffset: 0;
            verticalOffset: -3;
            radius: 8.0
            samples: 16
            transparentBorder: true;
            color: "#80000000"
            //opacity: 0.8;
        }
        */
    }


    StackView {
        id: stackView;
        anchors {
            left: menuBar.right;
            right: parent.right;
            bottom: parent.bottom;
            top: parent.top;
        }
        initialItem: inputSettings;
    }

    Component {
        id: audioSettings;
        Rectangle {
            color: settingsBubble.stackBackgroundColor;

            Column {
                anchors {
                    fill: parent;
                    topMargin: 25;
                }
                spacing: 25;

                Column {
                    id: videoHeader;
                    anchors {
                        left: parent.left;
                        //top: parent.top;
                        //topMargin: 25;
                        leftMargin: 25;
                    }
                    spacing: 5;

                    Text {
                        text: "Audio Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Edit the audio feed"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                        }
                    }
                }

                Column {
                    spacing: 25;
                    width: 200;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }

                    RowLayout {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        spacing: 25;
                        Text {
                            text: "Mute"
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }

                        Switch {
                            anchors.right: parent.right;
                        }
                    }
                }
            }
        }
    }

    Component {
        id: videoSettings;
        Rectangle {
            color: settingsBubble.stackBackgroundColor;

            Column {
                anchors {
                    fill: parent;
                    topMargin: 25;
                }
                spacing: 25;

                Column {
                    id: videoHeader;
                    anchors {
                        left: parent.left;
                        //top: parent.top;
                        //topMargin: 25;
                        leftMargin: 25;
                    }
                    spacing: 5;

                    Text {
                        text: "Video Settings"
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Edit the video output"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                        }
                    }
                }

                Column {
                    spacing: 25;
                    width: 200;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }

                    RowLayout {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        spacing: 25;
                        Text {
                            text: "VSync"
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }

                        Switch {
                            anchors.right: parent.right;
                        }
                    }

                    RowLayout {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }
                        spacing: 25;

                        Text {
                            text: "Auto Fullscreen"
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }

                        Switch {
                            anchors.right: parent.right;

                        }
                    }
                }

                Column {
                    id: filterHeader;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }
                    spacing: 5;

                    Text {
                        text: "Filtering";
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Enhance the output"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                        }
                    }
                }

                Column {
                    spacing: 25;
                    width: 200;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }

                    RowLayout {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        spacing: 25;
                        Text {
                            text: "Smooth"
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }

                        Switch {
                            anchors.right: parent.right;
                        }
                    }
                }

                Column {
                    id: aspectRatioHeader;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }
                    spacing: 5;

                    Text {
                        text: "Aspect Settings";
                        renderType: Text.QtRendering;
                        color: settingsBubble.textColor;
                        font {
                            family: "Sans";
                            pixelSize: 18;
                        }
                    }

                    Text {
                        text: "Adjust the output"
                        renderType: Text.QtRendering;
                        color: settingsBubble.alternateTextColor;
                        font {
                            family: "Sans";
                            pixelSize: 14;
                        }
                    }
                }

                Column {
                    spacing: 25;
                    width: 200;
                    anchors {
                        left: parent.left;
                        leftMargin: 25;
                    }

                    RowLayout {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        spacing: 25;
                        Text {
                            text: "Stretch Ratio"
                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 14;
                            }
                        }

                        Switch {
                            anchors.right: parent.right;
                        }
                    }
                }

            }

        }
    }

    Component {
        id: infoSettings;
        Rectangle {
            color: settingsBubble.stackBackgroundColor;

            Item {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: (parent.height - menuBar.height) - 25;

                Column {
                    id: column;
                    anchors.fill: parent;
                    spacing: 20;
                    property string textFieldColor: settingsBubble.textColor;
                    Column {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            leftMargin: 25;
                            rightMargin: 25;
                        }
                        spacing: 5;
                        Label {
                            text: "Library Path"
                            color: settingsBubble.textColor;
                            font.bold: true;
                        }

                        TextField {
                            anchors {
                                left: parent.left;
                                right: parent.right;
                                leftMargin: 25;
                                rightMargin: 25;
                            }

                            placeholderText: "C:/Users/lee/file.end";
                            width: 400;
                            style: TextFieldStyle {
                                textColor: settingsBubble.textColor;
                                placeholderTextColor: textColor;
                                background: Rectangle {
                                    color: column.textFieldColor;
                                }
                            }
                        }

                    }

                }
            }

        }
    }

    Component {
        id: inputSettings;
        Rectangle {
            color: settingsBubble.contentColor;
            Row {
                id: portRow;
                visible: stackView.width > width;
                anchors {
                    top: parent.top;
                    topMargin: 25;
                    horizontalCenter: parent.horizontalCenter;
                }
                spacing: 2;

                Button {
                    width: 100;
                    height: 35;
                    style: ButtonStyle {

                        background: Rectangle {
                            width: 100;
                            height: 35;
                            color: "darkgray";
                        }

                       label: Label {
                           text: "Player 1";
                           color: "#f1f1f1";
                           font {
                               pixelSize: 16;
                           }
                           horizontalAlignment: Text.AlignHCenter;
                           verticalAlignment: Text.AlignVCenter;
                       }
                    }
                }

                Button {
                    width: 100;
                    height: 35;
                    style: ButtonStyle {

                        background: Rectangle {
                            width: 100;
                            height: 35;
                            color: "darkgray";
                        }

                       label: Label {
                           text: "Player 2";
                           color: "#f1f1f1";
                           font {
                               pixelSize: 16;
                           }
                           horizontalAlignment: Text.AlignHCenter;
                           verticalAlignment: Text.AlignVCenter;
                       }
                    }
                }

                Button {
                    width: 100;
                    height: 35;
                    style: ButtonStyle {

                        background: Rectangle {
                            width: 100;
                            height: 35;
                            color: "darkgray";
                        }

                       label: Label {
                           text: "Player 3";
                           color: "#f1f1f1";
                           font {
                               pixelSize: 16;
                           }
                           horizontalAlignment: Text.AlignHCenter;
                           verticalAlignment: Text.AlignVCenter;
                       }
                    }
                }

                Button {
                    width: 100;
                    height: 35;
                    style: ButtonStyle {

                        background: Rectangle {
                            width: 100;
                            height: 35;
                            color: "darkgray";
                        }

                       label: Label {
                           text: "Player 4";
                           color: "#f1f1f1";
                           font {
                               pixelSize: 16;
                           }
                           horizontalAlignment: Text.AlignHCenter;
                           verticalAlignment: Text.AlignVCenter;
                       }
                    }
                }
            }

            Image {
                visible: stackView.width > width;
                anchors {
                    top: portRow.bottom;
                    right: inputMapper.left;
                    bottom: parent.bottom;
                    left: parent.left;
                    margins: 50;
                }
                fillMode: Image.PreserveAspectFit;
                sourceSize {
                    height: 500;
                    width: 500;
                }
                source: "../assets/retropad.png";
            }



            ScrollView {
                id: inputMapper;
                visible: stackView.width > width;
                anchors {
                    top: portRow.bottom;
                    right: parent.right;
                    bottom: parent.bottom;
                    bottomMargin: 25;
                    rightMargin: 25;
                    topMargin: 25;
                }
                width: 225;
                ListView {
                    anchors.fill: parent;
                    height: 500;
                    spacing: 5;
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
                        height: 35;
                        width: 225;
                        Label {
                            text: controllerButton;
                            anchors {
                                right: buttonField.left;
                                rightMargin: 15;
                            }

                            color: "#f1f1f1";
                            font {
                                pixelSize: 16;
                            }
                        }

                        TextField {
                            id: buttonField;
                            readOnly: true;
                            width: 100;
                            height: 25;
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

            //ComboBox {
                //width: 200;
               // model: inputmanager.enumerateDevices();
            //}

            Component.onCompleted: {
                //inputmanager.enumerateDevices();
            }
        }


    }

    Component {
        id: advancedSettings;
        Rectangle {
            color: settingsBubble.contentColor;

            Column {
                visible: stackView.width > width;
                anchors {
                    fill: parent;
                    topMargin: 25;
                }
                spacing: 15;

                CheckBox {
                    id: debugCheckBox;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    text: "Debug Mode";
                    checked: false;
                }

                GroupBox {
                    id: groupBox;
                    enabled: debugCheckBox.checked;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    height: 400;
                    width: parent.width * 0.85;
                    Rectangle {
                        anchors.fill: parent;
                        color: debugCheckBox.checked ? "darkgray" : "gray" ;

                        Column {
                            anchors.fill: parent;
                            Button {
                                text: "Button";
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: coreSettings;
        Rectangle {

            Component.onCompleted: {
                settingsDropDown.width = 500;
            }

            color: settingsBubble.contentColor;

            Column {
                id: coreHeaderColumn;
                spacing: 5;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    topMargin: 25;
                    leftMargin: 25;
                }

                Text {
                    id: coreTopLabel;
                    text: "Core Selection";
                    color: settingsBubble.textColor;
                    renderType: Text.QtRendering;
                    font {
                        pixelSize: 18;
                    }
                }

                Text {

                    text: "Tweak the emulation cores";
                    color: settingsBubble.alternateTextColor;
                    renderType: Text.QtRendering;
                    font {
                        pixelSize: 14;
                    }
                }

            }



            TableView {
                anchors {
                    top: coreHeaderColumn.bottom;
                    bottom: bottomBar.top;
                    left: parent.left;
                    right: parent.right;
                    bottomMargin: 15;
                    topMargin: 15;
                    leftMargin: 25;
                    rightMargin: 25;
                }

                model: ListModel {
                    ListElement {consoles: "Game Boy";}
                    ListElement {consoles: "Game Boy Advance";}
                    ListElement {consoles: "Game Boy Color";}
                    ListElement {consoles: "Nintendo";}
                    ListElement {consoles: "Super Nintendo";}
                    ListElement {consoles: "Sega Genesis";}
                    ListElement {consoles: "Nintendo 64";}
                    ListElement {consoles: "Sony PlayStation";}

                }

                style: TableViewStyle {
                    id: tableStyle;
                    backgroundColor: settingsBubble.contentColor;

                    rowDelegate: Rectangle {
                        color: tableStyle.backgroundColor;
                        height: 35;
                    }

                    frame: Item {

                    }


                    headerDelegate: Item {
                        height: 25;
                        Text {
                            anchors {
                                left: parent.left;
                            }

                            renderType: Text.QtRendering;
                            color: settingsBubble.textColor;
                            text: styleData.value;
                            font {
                                pixelSize: 16;
                                family: "Sans";
                            }
                        }
                    }
                }

                TableViewColumn {
                    role: "Console";
                    title: "Console";
                    width: 200;
                    delegate: Item {
                        anchors.fill: parent;
                        CheckBox {
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                right: parent.right;
                                rightMargin: 100;
                            }

                            text: styleData.value;
                            style: CheckBoxStyle {
                                label: Text {
                                    text: control.text;
                                    renderType: Text.QtRendering;
                                    color: settingsBubble.textColor;
                                    font {
                                        family: "Sans";
                                        pixelSize: 14;
                                    }
                                }
                            }
                        }
                    }
                }
                TableViewColumn {
                    role: "Core";
                    title: "Core";
                    width: 125;
                    delegate: Item {
                        anchors.fill: parent;
                        ComboBox {
                            height: 25;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                right: parent.right;
                                rightMargin: 15;
                            }

                            model: ["Apple", "Orange"];
                        }
                    }
                }
                TableViewColumn {
                    role: "performance";
                    title: "Performance";
                    width: 125;
                    delegate: Item {
                        anchors.fill: parent;
                        ComboBox {
                            height: 25;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                right: parent.right;
                                rightMargin: 15;
                            }

                            model: ["Apple", "Orange"];
                        }
                    }
                }

                TableViewColumn {
                    role: "version";
                    title: "Version";
                    width: 100;
                    delegate: Item {
                        anchors.fill: parent;
                        Text {
                            renderType: Text.QtRendering;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                                right: parent.right;
                                rightMargin: 25;
                            }
                            text: "1.0.0";
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 12;
                            }
                        }
                    }
                }

                TableViewColumn {
                    role: "author";
                    title: "Author";
                    width: 50;
                    delegate: Item {
                        anchors.fill: parent;
                        Text {
                            renderType: Text.QtRendering;
                            anchors {
                                verticalCenter: parent.verticalCenter;
                                left: parent.left;
                            }
                            text: "Unknown";
                            color: settingsBubble.textColor;
                            font {
                                family: "Sans";
                                pixelSize: 12;
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: bottomBar;
                color: settingsBubble.contentColor;
                height: 25;
                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    right: parent.right;
                    bottomMargin: 25;
                }

                Button {
                    id: saveButton;
                    text: "Save";
                    anchors {
                        left: parent.left;
                        bottom: parent.bottom;
                        leftMargin: 25;
                    }
                }

                Button {
                   anchors {
                       right: parent.right;
                       bottom: parent.bottom;
                       rightMargin: 25;
                   }

                   id: defaultButton;
                   text: "Default";
                }

            }
        }
    }

    Component {
        id: frontendSettings;
        Rectangle {
            color: settingsBubble.contentColor;
        }
    }

    Component {
        id: librarySettings;
        Rectangle {
            color: settingsBubble.contentColor;

            Row {
                spacing: 2;
                visible: stackView.width > width;
                anchors {
                    top: parent.top;
                    topMargin: 30;
                    horizontalCenter: parent.horizontalCenter;
                }
                Button {
                    text: "Import";
                    width: 100;
                    height: 30;
                    style: ButtonStyle {
                        background: Rectangle {
                            color: "lightgray";
                        }
                        label: Label {
                            text: "Import";
                            color: "#f1f1f1";
                            renderType: Text.QtRendering;
                            horizontalAlignment: Text.AlignHCenter;
                            verticalAlignment: Text.AlignVCenter;
                        }
                    }
                }

                Button {
                    text: "Backup";
                    width: 100;
                    height: 30;
                    style: ButtonStyle {
                        background: Rectangle {
                            color: "lightgray";
                        }
                        label: Label {
                            text: "Import";
                            renderType: Text.QtRendering;
                            color: "#f1f1f1";
                            horizontalAlignment: Text.AlignHCenter;
                            verticalAlignment: Text.AlignVCenter;
                        }
                    }
                }

                Button {
                    text: "Delete";
                    width: 100;
                    height: 30;
                    style: ButtonStyle {
                        background: Rectangle {
                            color: root.accentColor;
                        }
                        label: Label {
                            text: "Delete";
                            color: "#f1f1f1";
                            renderType: Text.QtRendering;
                            horizontalAlignment: Text.AlignHCenter;
                            verticalAlignment: Text.AlignVCenter;
                        }
                    }
                }
            }
        }
    }

    Component {
        id: saveSettings;

        Rectangle {
            color: settingsBubble.contentColor;

            Column {
                anchors {
                    topMargin: 15;
                    fill: parent;
                }
                spacing: 20;
                Label {
                    text: "Dropbox";
                    color: settingsBubble.textColor;
                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                    }
                    font {
                        family: "Sans";
                        pixelSize: 16;
                        bold: true;
                    }
                }

                Rectangle {
                    color: settingsBubble.groupingColor;
                    height: 125;
                    width: parent.width * 0.85;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    Column {
                        anchors.fill: parent;
                        anchors.topMargin: 10;
                        anchors.leftMargin: 25;
                        spacing: 5;
                        Row {
                            spacing: 57;
                            anchors {
                                left: parent.left;
                                right: parent.right;
                            }
                            height: 50;

                            Label {
                                text: "Login: ";
                                anchors.verticalCenter: parent.verticalCenter;
                                color: settingsBubble.textColor;
                                font {
                                    family: "Sans";
                                    pixelSize: 14;
                                    bold: true;
                                }
                            }

                            TextField {
                                anchors.verticalCenter: parent.verticalCenter;
                                width: parent.width * 0.5;
                                height: 25;
                            }
                        }

                        Row {
                            spacing: 25;
                            anchors {
                                left: parent.left;
                                right: parent.right;
                            }
                            height: 50;

                            Label {
                                text: "Password: ";
                                anchors.verticalCenter: parent.verticalCenter;
                                color: settingsBubble.textColor;
                                font {
                                    family: "Sans";
                                    pixelSize: 14;
                                    bold: true;
                                }
                            }

                            TextField {
                                width: parent.width * 0.5;
                                anchors.verticalCenter: parent.verticalCenter;
                                height: 25;
                                textColor: settingsBubble.textColor;
                            }
                        }
                    }
                }

                Label {
                    text: "Save Location";
                    color: settingsBubble.textColor;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    font {
                        family: "Sans";
                        pixelSize: 16;
                        bold: true;
                    }
                }

                Rectangle {
                    width: parent.width * 0.85;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    color: settingsBubble.groupingColor;
                    height: 75;

                    TextField {
                        id: savePathField;
                        text: "";
                        anchors {
                            fill: parent;
                            leftMargin: 25;
                            rightMargin: 25;
                            topMargin: 24;
                            bottomMargin: 24;
                        }
                    }
                }

                Label {
                    text: "Save States";
                    color: settingsBubble.textColor;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    font {
                        family: "Sans";
                        bold: true;
                        pixelSize: 16;
                    }
                }

                Rectangle {
                    height: 125;
                    width: parent.width * 0.85;
                    anchors.horizontalCenter: parent.horizontalCenter;
                    color: settingsBubble.groupingColor;

                    Column {
                        anchors.fill: parent;

                        Row {
                            height: 50;
                            anchors {
                                left: parent.left;
                                right: parent.right;
                            }

                            Label {
                                text: "Location: ";
                                color: settingsBubble.textColor;
                                font {
                                    family: "Sans";
                                    bold: true;
                                    pixelSize: 14;
                                }
                            }
                            TextField {
                                width: parent.width * 0.5;
                                textColor: settingsBubble.textColor;
                            }
                        }
                    }
                }
            }
        }
    }
}
