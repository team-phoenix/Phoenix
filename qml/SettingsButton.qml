import QtQuick 2.3
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Button {
    id: btn;
    exclusiveGroup: null;
    checked: false;
    checkable: true;
    height: 50;
    width: 50;
    property string backgroundColor: "#000000FF";
    text: "";

    style: ButtonStyle {
        background: Rectangle {
            height: 50;
            width: 50;


            id: backgroundRect;
            radius: 2;
            color: btn.backgroundColor;

        }



        label: Label {
            font.bold: true;
            font.pixelSize: 12;
            text: control.text;
            color: "#f1f1f1";

        }
    }
}
