import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Button {
    id: normalButton;
    text: "";
    property string textColor: "#f1f1f1";
    property string alternateTextColor: "#acacac";
    property Gradient buttonGradient: Gradient {
        //GradientStop {position: 0.0; color: "#131313";}
        GradientStop {position: 0.0; color: checked ? "#000000" : "#333332";}
        GradientStop {position: 0.5; color: checked ? "#0f0f0f" : "#232322";}
        GradientStop {position: 0.75; color: checked ? "#1b1c1c" : "#151615";}
        GradientStop {position: 1.0; color: checked ? "#1e1f1f" : "#070908";}
    }

    property Gradient innerBorderGradient: Gradient {
        GradientStop {position: 0.0; color: "#4f4f4f";}
        //GradientStop {position: 0.05; color: "#303030";}
        GradientStop {position: 0.06; color: checked ? "#000000" : "#4f4f4f";}
        GradientStop {position: 0.35; color: checked ? "#000000" : "#333332";}
        GradientStop {position: 0.5; color: checked ? "#0f0f0f" : "#232322";}
        GradientStop {position: 0.95; color: checked ? "#1b1c1c" : "#151615";}
        GradientStop {position: 1.0; color: checked ? "#40403f" : "#080908";}
    }

    property string outerBorderColor: "#40403f";
    property string alternateOuterBorderColor: "#1a1a1a"
    property int radius: 0;

    style: ButtonStyle {
        background: Rectangle {
            id: outerRectangle;
            implicitHeight: 25;
            implicitWidth: 50;
            radius: normalButton.radius;

            gradient: control.innerBorderGradient;

            CustomBorder {
                radius: parent.radius;
                color: "#0f0f0f";
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
