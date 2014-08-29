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

            Rectangle {
                // topBorder;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                radius: 3;
                height: 1;
                color: "#1f1f1f";
            }

            Rectangle {
                // leftBorder;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    left: parent.left;
                }
                radius: 3;
                width: 1;
                color: "#141414";
            }

            Rectangle {
                // rightBorder;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    right: parent.right;
                }
                radius: 3;
                width: 1;
                color: "#141414";
            }

            Rectangle {
                // bottomBorder;
                radius: 3;

                anchors {
                    bottom: parent.bottom;
                    left: parent.left;
                    right: parent.right;
                }
                height: 1;
                color: "#3b3b3b";
            }
        }
    }
}
