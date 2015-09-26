import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import vg.phoenix.themes 1.0
import vg.phoenix.models 1.0

TextField {
    id: searchBar;
    height: 35;
    placeholderText: "";
    width: 250;
    textColor: "white";

    style: TextFieldStyle {
        placeholderTextColor: PhxTheme.common.baseFontColor;
        background: Item {
            width: control.width;
            height: control.height;

            Rectangle {
                anchors.fill: parent;
                color: "transparent";
                border.width: 2;
                border.color: "#FFFFFF";
                opacity: .35;
            }
        }
    }

}
