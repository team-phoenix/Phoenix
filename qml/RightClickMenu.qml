import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: dropdownMenu;
    width: 100;
    height: 200;
    color: "red";
    radius: 3;

    ListView {
        height: parent.height;
        width: parent.width;
        model: ListModel {
            ListElement {title: "Play";}
           // ListElement {title: "Compress";}
            ListElement {title: "Remove File";}
        }

        delegate: Button {
            height: 25;
            width: 100;
            text: title;
            onClicked: {
                if (title == "Play") {
                    var curItem = gridView.currentItem;
                    var core = phoenixLibrary.getSystem(curItem.systemName);
                    root.gameAndCoreCheck(curItem.titleName, curItem.systemName, curItem.fileName, core);
                }

                else if (title == "Remove File") {
                    phoenixLibrary.deleteRow(gridView.currentItem.titleName);
                }
            }
        }
    }


}
