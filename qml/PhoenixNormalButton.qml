import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Button {
    id: normalButton;
    text: "";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#acacac";
    property Gradient buttonGradient: Gradient {
        GradientStop {position: 0.0; color: checked ? "#171717" : "#2f2f2f";}
        GradientStop {position: 0.8; color: "#252324";}
        GradientStop {position: 1.0; color: "#1b1b1b";}
    }

    property Gradient innerBorderGradient: Gradient {
        GradientStop {position: 0.0; color: "#454547";}
        GradientStop {position: 1.0; color: "#262626";}
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
                visible: !control.checked;
                color: control.checked ? control.outerBorderColor : control.alternateOuterBorderColor;
            }

            Rectangle {
                id: innerRectangle;
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
