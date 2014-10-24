import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Button {
    id: normalButton;
    text: "";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#acacac";
    property Gradient buttonGradient: Gradient {
        GradientStop {position: 0.0; color: checked ? "#1c1c1d" : "#4c4b4d";}
        GradientStop {position: 0.5; color: checked ? "#262626" : "#424142";}
        GradientStop {position: 1.0; color: checked ? "#2f2e2f" : "#363636";}
    }

    property Gradient innerBorderGradient: Gradient {
        GradientStop {position: 0.0; color: "#646466";}
        GradientStop {position: 0.05; color: "#49494c";}
        GradientStop {position: 1.0; color: "#292929";}
    }

    property string outerBorderColor: "#121212";
    property string alternateOuterBorderColor: "#1a1a1a"
    property int radius: 3;

    style: ButtonStyle {
        background: Rectangle {
            id: outerRectangle;
            implicitHeight: 25;
            implicitWidth: 50;
            radius: normalButton.radius;
            border {
                width: control.checked ? 1 : 0;
                color: control.outerBorderColor;
            }

            gradient: control.innerBorderGradient;

            CustomBorder {
                radius: parent.radius;
                gradient: Gradient {
                    GradientStop {position: 0.0; color: control.checked ? "#2f2f2f" : "black";}
                    GradientStop {position: 0.05; color: "black";}
                    GradientStop {position: 0.90; color: "black";}
                    GradientStop {position: 1.0; color: control.checked ? "#4e4e51" : "black";}
                }
            }

            Rectangle {
                id: innerRectangle;
                radius: parent.radius;
                anchors {
                    fill: parent;
                    margins: 1;
                }
                gradient: control.buttonGradient;

            }
        }
        label: Text {
            renderType: Text.QtRendering;
            color: control.checked ? control.alternateTextColor : control.textColor;
            text: control.text;
            verticalAlignment: Text.AlignVCenter;
            horizontalAlignment: Text.AlignHCenter;
        }
    }
}
