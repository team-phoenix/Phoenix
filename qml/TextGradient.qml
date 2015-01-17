import QtQuick 2.3
import QtGraphicalEffects 1.0


Item {
    id: gradientText;
    property int pointSize: 16;
    property bool dropShadow: true;
    property string text: qsTr("Audio");
    property bool bold: true;
    property string textColor: "black";
    property int elide: Text.ElideNone;
    property bool enableGradient: true;

    property Gradient gradient: Gradient {
        GradientStop { position: 0.0; color: "#f06612";}
        GradientStop { position: 1.0; color: "#dc113b";}
    }

    Text {
        id: textMask;
        renderType: Text.QtRendering;
        text: parent.text;
        anchors {
            left: parent.left;
            verticalCenter: parent.verticalCenter;
        }

        font {
            family: "Sans";
            bold: gradientText.bold;
            pointSize: gradientText.pointSize;
        }
        visible: !parent.enableGradient;
        color: parent.textColor;
        elide: parent.elide;
        horizontalAlignment: Text.AlignLeft;
    }

    Rectangle {
        id: gradientSource;
        anchors {
            fill: textMask;
        }
        visible: false;
        gradient: gradientText.gradient;
    }


   OpacityMask {
       id: opacityMask;
       visible: parent.enableGradient;
       anchors {
           fill: gradientSource;
       }
       source: gradientSource;
       maskSource: textMask;
   }

   DropShadow {
       visible: gradientText.dropShadow && parent.enableGradient;
       source: opacityMask;
       anchors.fill: source;
       horizontalOffset: 1;
       verticalOffset: 1
       radius: 2.0
       samples: radius * 2;
       color: "black";
       spread: 0.5;
   }
}
