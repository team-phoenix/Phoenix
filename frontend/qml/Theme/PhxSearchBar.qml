import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

TextField {
    id: searchBar;
    width: 225;
    height: 50;
    font {
        pixelSize: 14;
    }

    state: "EXPANDED";

    transitions: Transition {
        NumberAnimation { properties: "width"; duration: 300; }
    }

    states: [
        State {
            name: "ICON_ONLY";
            PropertyChanges {
                target: searchBar;
                width: 42;
            }
        },
        State {
            name: "EXPANDED";
            PropertyChanges {
                target: searchBar;
                width: 150;
            }
        }
    ]

    /*
    MouseArea {
        anchors.fill: parent;
        onClicked: {
            if ( parent.state === "ICON_ONLY" )
                parent.state = "EXPANDED";
            else
                parent.state = "ICON_ONLY";
        }
    }*/

    textColor: root.normalFontColor;
    style: TextFieldStyle {
        background: Rectangle {
            color: "red";
            implicitHeight: control.height;
            implicitWidth: control.width;
        }
    }

    placeholderText: qsTr( "Search" );

    Timer {
        id: searchTimer;
        interval: 300;
        running: false;
        repeat: false;
        onTriggered: {
            libraryModel.setFilter( "title LIKE ?", [ '%' + searchBar.text + '%' ], true );
        }

    }
    onTextChanged: searchTimer.restart();
}
