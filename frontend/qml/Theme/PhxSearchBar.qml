import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import vg.phoenix.themes 1.0
import vg.phoenix.models 1.0

TextField {
    id: searchBar;

    height: 25;

    placeholderText: "";
    width: 200;
    textColor: "white";

    style: TextFieldStyle {
        placeholderTextColor: PhxTheme.common.baseFontColor;
        background: Item {
            width: control.width;
            height: control.height;

            Rectangle {
                anchors.fill: parent;
                color: "#1a1b20";
                radius: 3;
                border {
                    width: 1;
                    color: "#111216";
                }

                Rectangle {
                    z: parent.z - 1;
                    radius: parent.radius;
                    anchors {
                        fill: parent;
                        bottomMargin: -1;
                    }
                    color: "white";
                    opacity: 0.1;

                }
            }
        }
    }

}
