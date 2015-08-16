import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

import vg.phoenix.models 1.0
import vg.phoenix.themes 1.0

Rectangle {
    id: contentArea;
    height: 200;
    width: 200;
    color: "#f1f1f1"// "gray";

    property alias contentLibraryModel: libraryModel;
    property alias contentStackView: contentAreaStackView;
    property alias contentInputView: inputView;

    HeaderBar {
        id: headerBar;
        z: 100;
        height: 36;
        state: "SELECTION";

        anchors {
            left: parent.left
            right: parent.right;
            top: parent.top;

            topMargin: 24;
            leftMargin: 12;
            rightMargin: 12;
        }

        /*
        visible: {
            if ( contentAreaStackView.currentItem !== null ) {
                return contentAreaStackView.currentItem.objectName === "BoxartGridView";
            }
            return false;
        }*/


        /*MouseArea {
            anchors.fill: parent;
            onClicked: headerBar.state = "PLAYING_GAME";
        }
        */
    }

    LibraryModel {
        id: libraryModel;

        function dragEvent( drag ) {
            if ( drag.hasUrls ) {
                handleDraggedUrls( drag.urls );
            }
        }

        function dropEvent( drop ) {
            handleDroppedUrls();
        }

        function containsEvent() {
            handleContainsDrag( rootDropArea.containsDrag );
        }

        Component.onCompleted: {
            rootDropArea.onEntered.connect( dragEvent );
            rootDropArea.onDropped.connect( dropEvent );
            rootDropArea.onContainsDragChanged.connect( containsEvent );

            libraryModel.startWorkerThread();
        }
    }

    StackView {
        id: contentAreaStackView;
        initialItem: boxArtGridComponent;

        anchors {
            fill: parent;
        }

        delegate: StackViewDelegate {
            function transitionFinished( properties )
            {
                properties.exitItem.opacity = 1;
            }

            pushTransition: StackViewTransition {
                PropertyAnimation {
                    target: enterItem;
                    property: "opacity";
                    from: 0;
                    to: 1;
                }
                PropertyAnimation {
                    target: exitItem;
                    property: "opacity";
                    from: 1;
                    to: 0;
                }
            }
        }
    }

    Component {
        id: boxArtGridComponent;

        BoxartGridView {
            id: boxartGrid;

            color: "transparent" //"gray";

            objectName: "BoxartGridView";

        }
    }

    Component {
        id: detailGameViewComponent;


        DetailedGameView {

        }
    }

    Component {
        id: inputView;

        InputView {

            objectName: "InputView"

        }
    }


}
