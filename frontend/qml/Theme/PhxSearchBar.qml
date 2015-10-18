import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import vg.phoenix.themes 1.0
import vg.phoenix.models 1.0

TextField {
    id: searchBar;
    placeholderText: "";
    textColor: "#333";

    style: TextFieldStyle {
        placeholderTextColor: PhxTheme.common.baseFontColor;
        background: Item {
            width: control.width;
            height: control.height;
        }
    }
}
