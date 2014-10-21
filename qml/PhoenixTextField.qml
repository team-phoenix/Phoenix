import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

TextField {
    id: searchBar;
    width: 175;
    height: 25;
    placeholderText: "Sample";
    visible: headerBar.searchBarVisible;
    font {
        pixelSize: 12;
    }
    textColor: "#f1f1f1";

    style: TextFieldStyle {
        placeholderTextColor: "#f1f1f1";
        renderType: Text.QtRendering;

        background: Rectangle {
            radius: 3;
            color: "#1a1a1a";
            height: 25;
            width: 175;

            CustomBorder {
                gradient: Gradient {
                    GradientStop {position: 0.05; color: "#1f1f1f";}
                    GradientStop {position: 0.9; color: "#141414";}
                    GradientStop {position: 0.95; color: "#3b3b3b";}
                }
            }
        }
    }
}
