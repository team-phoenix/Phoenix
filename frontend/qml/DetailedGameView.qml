import QtQuick 2.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.1

Item {
    anchors.fill: parent;
    width: 100
    height: 62

    Rectangle {
        anchors.fill: parent;
        color: "red";
        opacity: 0.5;

        Text {
            text: qsTr( "Super Mario Bros." );
        }
    }
}

