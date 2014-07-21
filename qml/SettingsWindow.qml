import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: settingsWindow;
    width: 500;
    height: 500;

    minimumWidth: 640;
    minimumHeight: 480;
    maximumHeight: minimumHeight;
    maximumWidth: minimumWidth;

    Item {
        anchors.fill: parent;

        Rectangle {
            id: menuBar;
            z: stackView.z + 1;
            color: "#333333";
            anchors {
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 150;



            ExclusiveGroup {
                id: menuGroup;
                property string buttonBackgroundColor: "#000000FF";
            }



            ListView {
                id: listView;
                anchors {
                    top: parent.top;
                    bottom: advancedRectangle.top;
                    left: parent.left;
                    right: parent.right;
                    topMargin: 20;
                    //bottomMargin: 50;
                }
                highlightFollowsCurrentItem: false;
                spacing: 15;
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
                            id: accentRectangle;
                            color: root.accentColor;
                            width: 15;
                            anchors {
                                left: parent.left;
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                        }


                        Rectangle {
                            id: mainColor;
                            anchors {
                                left: accentRectangle.right;
                                right: parent.right;
                                top: parent.top;
                                bottom: parent.bottom;
                            }
                            color: listView.currentItem ? "#666666" : "#000000FF";
                        }


                        Image {
                            id: image;
                            anchors {
                                left: mainColor.right;
                            }
                            source: "../assets/triangle.png";
                            sourceSize {
                                height: 20;
                                width: 30;
                            }
                            height: highlightItem.height;
                            width: 30;
                        }

                    }

                    DropShadow {
                        width: innerItem.width + 30;
                        height: innerItem.height + 5;

                        horizontalOffset: 1;
                        verticalOffset: 1;
                        radius: 8.0
                        samples: 16
                        transparentBorder: true;
                        color: "#80000000"
                        opacity: 0.8;
                        source: innerItem;
                    }
                }

                property var stacks: { "Input": inputSettings,
                                       "Cores": coreSettings,
                                       "Info": infoSettings,
                                       "Library": librarySettings,
                                       "Save": saveSettings,
                                       "Advanced": advancedSettings};
                model: ListModel {
                    ListElement {title: "Input";}
                    //ListElement {title: "System";}
                    ListElement {title: "Library";}
                    ListElement {title: "Save";}
                    ListElement {title: "Cores";}
                    ListElement {title: "Advanced";}
                    //ListElement {title: "Developer";}
                    ListElement {title: "Themes";}


                }

                delegate: Label {
                    height: 35;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        //top: parent.top;
                        //topMargin: 25;
                    }
                    font {
                        family: "Sans";
                        bold: true;
                        pixelSize: 16;
                    }

                    color: "#f1f1f1";
                    text: title;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            //downArrow.source = "";
                            listView.currentIndex = index;
                            //downArrow.source = "assets/arrow-down.png";
                            stackView.clear();
                            stackView.push(listView.stacks[text]);
                        }
                    }
                }
            }

            Rectangle {
                id: advancedRectangle;
                z: listView.z + 1;
                height: 75;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }

                color: "#242323";


                Image {
                    anchors.centerIn: parent;
                    source: "../assets/more.png";
                    sourceSize {
                        width: 40;
                        height: 40;
                    }
                    height: 40;
                    width: 40;
                }
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
        }


        StackView {
            id: stackView;
            anchors {
                left: menuBar.right;
                right: parent.right;
                bottom: parent.bottom;
                top: parent.top;
            }

            initialItem: infoSettings;
        }

        InnerShadow {
            source: stackView;
            anchors.fill: source;
            radius: 8.0;
            samples: 16;
            horizontalOffset: 4;
            verticalOffset: 3;
            color: Qt.rgba(0, 0, 0, 0.3);
        }

        Component {
            id: infoSettings;
            Rectangle {
                color: "#555555";

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
                        property string textFieldColor: "#1f1f1f";
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
                                color: "#f1f1f1";
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
                                    textColor: "#f1f1f1";
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
                color: "#363434";

                Label {
                    text: "RetroPad";
                    color: "#f1f1f1";
                    anchors {
                        top: parent.top;
                        topMargin: 25;
                        horizontalCenter: parent.horizontalCenter;
                    }
                    font {
                        family: "Sans";
                        pixelSize: 21;
                    }
                }


                /*Rectangle{
                    anchors {
                        top: parent.top;
                        right: parent.right;
                    }
                    width: parent.width / 2;
                    height: 50;
                    color: "#232323";

                    Label {
                        anchors.centerIn: parent;
                        text: "Super Nintendo";
                        font.bold: true;
                        font.pixelSize: 11;
                        color: "#f1f1f1";
                    }
                }*/

                Image {
                    id: inputArt;
                    source: "../assets/blank_retropad.png";
                    sourceSize {
                        height: 500;
                        width: 500;
                    }

                    anchors {
                        bottom: parent.bottom;
                    }
                    fillMode: Image.PreserveAspectFit;
                    width: parent.width;
                    height: parent.height * 0.8;
                }

                Image {
                    id: button1;
                    source: clicked ? "../assets/purple_button.png" : "";
                    sourceSize {
                        height: height;
                        width: width;
                    }
                    height: 32;
                    width: 32;

                    anchors {
                        centerIn: parent;
                        verticalCenterOffset: 6;
                        horizontalCenterOffset: 70;

                    }

                    property bool clicked: false;

                    MouseArea {
                        id: mouse1;
                        anchors.fill: parent;
                        onClicked: {
                            if (button1.clicked)
                                button1.clicked = false;
                            else
                                button1.clicked = true;
                        }
                    }

                    Label {
                        visible: button1.clicked;
                        anchors.centerIn: parent;
                        color: "#f1f1f1";
                        text: "Y";
                        font {
                            family: "Sans";
                            pixelSize: 16;
                        }
                    }
                }

                DropShadow {
                    source: button1;
                    anchors.fill: source;
                    visible: !(button1.source == "");
                    horizontalOffset: 2;
                    verticalOffset: 2;
                    radius: 8.0
                    samples: 16
                    transparentBorder: true;
                    color: "#b0000000"
                    //opacity: 0.8;
                }

                Image {
                    id: button2;
                    source: "";
                    sourceSize {
                        height: height;
                        width: width;
                    }
                    height: 33;
                    width: 33;

                    anchors {
                        centerIn: parent;
                        verticalCenterOffset: -23;
                        horizontalCenterOffset: 105;

                    }

                    property bool clicked: false;
                    MouseArea {
                        id: mouse2;
                        anchors.fill: parent;
                        onClicked: {
                            if (button2.clicked) {
                                button2.source = "";
                                button2.clicked = false;
                            }
                            else {
                                button2.clicked = true;
                                button2.source = "../assets/blue_button.png";
                            }
                        }
                    }

                    Label {
                        visible: button2.clicked;
                        anchors.centerIn: parent;
                        color: "#f1f1f1";
                        text: "X";
                        font {
                            family: "Sans";
                            pixelSize: 16;
                        }
                    }
                }

                DropShadow {
                    source: button2;
                    anchors.fill: source;
                    horizontalOffset: 2;
                    visible: !(button2.source = "");
                    verticalOffset: 2;
                    radius: 8.0
                    samples: 16
                    transparentBorder: true;
                    color: "#b0000000"
                    //opacity: 0.8;
                }

                Image {
                    id: button3;
                    source: clicked ? "../assets/orange_button.png" : "";
                    sourceSize {
                        height: height;
                        width: width;
                    }
                    height: 32;
                    width: 32;

                    anchors {
                        centerIn: parent;
                        verticalCenterOffset: 37;
                        horizontalCenterOffset: 105;

                    }

                    property bool clicked: false;
                    MouseArea {
                        id: mouse3;
                        anchors.fill: parent;
                        onClicked: {
                            if (button3.clicked)
                                button3.clicked = false;
                            else
                                button3.clicked = true;
                        }
                    }

                    Label {
                        visible: button3.clicked;
                        anchors.centerIn: parent;
                        color: "#f1f1f1";
                        text: "B";
                        font {
                            family: "Sans";
                            pixelSize: 16;
                        }
                    }
                }

                DropShadow {
                    source: button3;
                    anchors.fill: source;
                    horizontalOffset: 2;
                    visible: !(button3.source == "");
                    verticalOffset: 2;
                    radius: 8.0
                    samples: 16
                    transparentBorder: true;
                    color: "#b0000000"
                    //opacity: 0.8;
                }

                Image {
                    id: button4;
                    source: "";
                    sourceSize {
                        height: height;
                        width: width;
                    }
                    height: 33;
                    width: 33;

                    anchors {
                        centerIn: parent;
                        verticalCenterOffset: 7;
                        horizontalCenterOffset: 141;

                    }

                    property bool clicked: false;
                    MouseArea {
                        id: mouse4;
                        anchors.fill: parent;
                        onClicked: {
                            if (button4.clicked) {
                                button4.clicked = false;
                                button4.source = "";
                            }
                            else {
                                button4.clicked = true;
                                button4.source = "../assets/red_button.png";

                            }
                        }
                    }

                    Label {
                        visible: button4.clicked;
                        anchors.centerIn: parent;
                        color: "#f1f1f1";
                        text: "A";
                        font {
                            family: "Sans";
                            pixelSize: 16;
                        }
                    }
                }

                DropShadow {
                    source: button4;
                    anchors.fill: source;
                    horizontalOffset: 2;
                    visible: !(button4.source = "");
                    verticalOffset: 2;
                    radius: 8.0
                    samples: 16
                    transparentBorder: true;
                    color: "#b0000000"
                    //opacity: 0.8;
                }


                Button {
                    id: btn;
                    anchors.top: inputArt.top;
                    anchors.right: inputArt.right;
                    anchors.rightMargin: 181;
                    anchors.topMargin: 101;
                    height: 23;
                    width: 23;

                    property string imageSource: btn.pressed ? "red.png" : "";

                    style: ButtonStyle {
                        background: Rectangle {
                            color: "#000000FF";
                        }

                        label: Item {
                            Image {
                                id: image;
                                source: btn.imageSource;
                            }
                        }
                    }
                }
            }
        }

        Component {
            id: advancedSettings;
            Rectangle {
                color: "lightgray";

                Column {
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
            StackView {
                id: coreStack;
                initialItem: coreOptions;

                Component {
                    id: coreTable;
                    TableView {
                        TableViewColumn{role: "title"; title: "Title"; width: 150;}
                        TableViewColumn{role: "console"; title: "Console"; width: 200;}
                        TableViewColumn{role: "author"; title: "Author"; width: 200;}

                            model: ListModel {
                                ListElement{title: "Snes9x"; author: "phx"; console: "Super Nintendo";}
                                ListElement{title: "bSNES (Accuracy)"; author: "Byuu"; console: "Super Nintendo";}
                                ListElement{title: "bSNES (Balanced)"; author: "Byuu"; console: "Super Nintendo";}
                                ListElement{title: "bSNES (Performance)"; author: "Byuu"; console: "Super Nintendo";}
                                ListElement{title: "Mupen64Plus"; author: "Unknown"; console: "Nintendo 64";}
                                ListElement{title: "PCSX Reloaded"; author: "Unknown"; console: "Sony PlayStation";}
                                ListElement{title: "Nestopia"; author: "Unknown"; console: "Nintendo";}
                                ListElement{title: "QuickNES"; author: "Unknown"; console: "Nintendo";}
                                ListElement{title: "Fceumm"; author: "Unknown"; console: "Nintendo";}

                       }
                    }

                }

                Component {
                    id: coreOptions;
                    Rectangle {
                        color: "lightgray";

                        Button {
                            id: infoButton;
                            text: ">>";
                            height: 20;
                            width: 20;
                            anchors {
                                top: parent.top;
                                right: parent.right;
                                topMargin: 10;
                                rightMargin: 10;
                            }

                            onClicked: {
                                coreStack.push({item: coreTable,replace: true});
                            }
                        }

                        ListView {
                            anchors {
                                fill: parent;
                                topMargin: 25;
                            }

                            model: ListModel {
                                ListElement {title: "Nintendo Entertainment System";}
                                ListElement {title: "Super Nintendo";}
                                ListElement {title: "Nintendo 64";}
                                ListElement {title: "Sony PlayStation";}
                                ListElement {title: "Game Boy Advance";}

                            }

                            delegate: Item {
                                id: coreDelegate;
                                height: 175;
                                anchors {
                                    left: parent.left;
                                    right: parent.right;

                                }

                                Column {
                                    anchors.fill: parent;
                                    spacing: 15;

                                    Label {
                                        anchors.horizontalCenter: parent.horizontalCenter;
                                        text: title;
                                        font {
                                            family: "Sans";
                                            bold: true;
                                            pixelSize: 16;
                                        }

                                    }

                                    Rectangle {
                                        color: "darkgray";
                                        height: 125;
                                        width: parent.width * 0.85;
                                        anchors.horizontalCenter: parent.horizontalCenter;

                                    }

                                }
                            }
                        }
                    }
                }
            }
        }

        Component {
            id: frontendSettings;
            Rectangle {
                color: "green";
            }
        }

        Component {
            id: librarySettings;
            Rectangle {
                color: "lightgray";
                Column {
                    anchors {
                        fill: parent;
                        topMargin: 15;
                    }
                    spacing: 15;

                    Label {
                        text: "Library Locations";
                        anchors.horizontalCenter: parent.horizontalCenter
                        font {
                            family: "Sans";
                            bold: true;
                            pixelSize: 16;
                        }
                    }

                    ComboBox {
                        anchors.horizontalCenter: parent.horizontalCenter
                        model: ["/home/lee/Desktop", "/user/path"];
                    }

                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter;
                        height: 150;
                        width: parent.width * 0.85;
                        color: "darkgray";
                        Column {
                            anchors.fill: parent;
                            spacing: 15;
                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter;
                                text: "Import";
                            }

                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter;
                                text: "Backup";
                            }
                            Button {
                                anchors.horizontalCenter: parent.horizontalCenter;
                                text: "Delete";
                            }
                        }
                    }
                }
            }
        }

        Component {
            id: saveSettings;

            Rectangle {
                color: "lightgray";

                Column {
                    anchors {
                        topMargin: 15;
                        fill: parent;
                    }
                    spacing: 20;
                    Label {
                        text: "Dropbox";
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
                        color: "darkgray";
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
                                }
                            }
                        }
                    }

                    Label {
                        text: "Save Location";
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

                        color: "darkgray";
                        height: 75;

                        TextField {
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
                        color: "darkgray";

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
                                    font {
                                        family: "Sans";
                                        bold: true;
                                        pixelSize: 14;
                                    }
                                }
                                TextField {
                                    width: parent.width * 0.5;
                                }
                            }
                        }
                    }



                }

            }
        }
    }




}
