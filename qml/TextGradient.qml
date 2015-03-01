import QtQuick 2.3
import QtGraphicalEffects 1.0


Item {
    id: gradientText;
    property int pointSize: 16;
    property bool dropShadow: true;
    property string text: qsTr("Audio");
    property bool bold: true;
    property string textColor: "#f1f1f1";
    property int elide: Text.ElideNone;
    property bool enableGradient: true;

    property Gradient gradient: Gradient {
        GradientStop { position: 0.0; color: "#f06612";}
        GradientStop { position: 1.0; color: "#dc113b";}
    }

    Text {
        id: textMask;
        z: dropShadow.z + 1;
        renderType: Text.QtRendering;
        text: parent.text;
        anchors {
            left: parent.left;
            leftMargin: index === listView.currentIndex ? 12 : 0;
            verticalCenter: parent.verticalCenter;
        }

        font {
            family: "Sans";
            bold: gradientText.bold && root.consoleBarFocus;
            pointSize: gradientText.pointSize;
        }
        color: index === listView.currentIndex && root.consoleBarFocus ? "#872018" : parent.textColor;
        elide: parent.elide;
        horizontalAlignment: Text.AlignLeft;
    }

    DropShadow {
        id: dropShadow;
        visible: index === listView.currentIndex;
        anchors.fill: source;
        source: textMask;
        color: "white";
        opacity: 0.35;
        verticalOffset: 1;
        horizontalOffset: 0;
        samples: radius * 2;
        radius: 1;
    }

}
