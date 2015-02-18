import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

TextField {
    id: searchBar;
    width: 175;
    height: 25;
    placeholderText: "Hello World!";
    property int radius: 3;
    property string color: "red";
    property string borderColor: "black";
    property int renderType: Text.QtRendering;
    property color placeholderTextColor: "white";
    property int borderWidth: 1;
    textColor: "#f1f1f1";

    style: TextFieldStyle {
        placeholderTextColor: control.placeholderTextColor;
        renderType: control.renderType;

        background: Rectangle {
            radius: control.radius;
            height: control.height;
            width: control.width;
            border {
                width: control.borderWidth;
                color: control.borderColor;
            }

            gradient: Gradient {
                GradientStop {position: 0.0; color: "#191919";}
                GradientStop {position: 0.40; color: "#2b2b2c";}
                GradientStop {position: 1.0; color: "#2e2e2e";}
            }
            CustomBorder {
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "#1d1d1d";}
                    GradientStop {position: 0.05; color: "#2e2e2e";}
                    GradientStop {position: 0.90; color: "#2e2e2e";}
                    GradientStop {position: 1.0; color: "#4e4e51";}
                }
            }
        }
    }
}
