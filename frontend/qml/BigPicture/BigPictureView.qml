import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

import vg.phoenix.themes 1.0
import vg.phoenix.models 1.0
import vg.phoenix.cache 1.0
import vg.phoenix.backend 1.0


Rectangle {
    id: bigPictureView;
    color: PhxTheme.bigPictureView.baseColor;

    property QMLInputDevice qmlInputDevice: QMLInputDevice {

        onLeftChanged: {
            if ( left )
            focusMover( InputDeviceEvent.Left );
        }

        onRightChanged: {
            if ( right )
            focusMover( InputDeviceEvent.Right);

        }

        onUpChanged: {
            if ( up )
            focusMover( InputDeviceEvent.Up );

        }

        onDownChanged: {
            if ( down )
            focusMover( InputDeviceEvent.Down );

        }
    }

    function focusMover( event ) {
        if ( repeater.focus ) {
            repeater.changePosition( event );
        } else if ( sectionRow.focus ) {
            sectionRow.changePosition( event );
        }

    }



    property LibraryModel libraryModel: LibraryModel {
        id: libraryModel;

        /*
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
        */
        Component.onCompleted: {
            //rootDropArea.onEntered.connect( dragEvent );
            //rootDropArea.onDropped.connect( dropEvent );
            //rootDropArea.onContainsDragChanged.connect( containsEvent );

            libraryModel.startWorkerThread();
        }
    }

    Column {
        id: bigPictureViewColumn;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }
        height: 150;

        spacing: 24;

        InfoBar {
            anchors {
                left: parent.left;
                right: parent.right;
            }

            height: 42;
            spacing: 0;

        }

        Row {
            id: sectionRow;
            anchors {
                horizontalCenter: parent.horizontalCenter;
            }
            spacing: 24;

            function changePosition( event ) {
                if ( event === InputDeviceEvent.Right ) {
                    if ( gamesButton.checked ) {
                        rightTempButton.checked = true;
                    } else if ( leftTempButton.checked ) {
                        gamesButton.checked = true;
                    }
                } else if ( event === InputDeviceEvent.Left ) {
                    if ( gamesButton.checked ) {
                        leftTempButton.checked = true;
                    } else if ( rightTempButton.checked ) {
                        gamesButton.checked = true;
                    }
                } else if ( event === InputDeviceEvent.Down ) {
                    sectionRow.focus = false;
                    repeater.focus = true;
                }
            }

            ExclusiveGroup {
                id: sectionGroup;
            }

            SectionButton {
                id: leftTempButton;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                height: bigPictureView.height * 0.07;
                width: bigPictureView.width * 0.15;
                exclusiveGroup: sectionGroup;


            }

            SectionButton {
                id: gamesButton;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                height: bigPictureView.height * 0.07;
                width: bigPictureView.width * 0.15;

                checked: true;
                exclusiveGroup: sectionGroup;
                text: qsTr( "Games" );
            }

            SectionButton {
                id: rightTempButton;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                height: bigPictureView.height * 0.07;
                width: bigPictureView.width * 0.15;
                exclusiveGroup: sectionGroup;
            }
        }

    }


    Rectangle {
        visible: true;
        anchors {
            top: bigPictureViewColumn.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

        color: "transparent";
        height: 700;
        border {
            width: 3;
            color: "white";
        }

        Component {
            id: listView;

            ListView {
                id: bladeListView;
                spacing: 24;
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    left: parent.left;
                    right: parent.right;
                    bottomMargin: 50;
                    leftMargin: 24;
                }

                model: bigPictureView.libraryModel;

                orientation: ListView.Horizontal;

                delegate: Rectangle {

                    color: "blue"
                    anchors {
                        top: parent.top;
                        bottom: parent.bottom;
                    }

                    width: 300;

                    DropShadow {
                        visible: true;
                        anchors.fill: source;
                        source: gridItemImage;
                        verticalOffset: index === bladeListView.currentIndex ? 6 : 4;
                        horizontalOffset: 0;
                        color: "black";
                        transparentBorder: true;
                        samples: radius * 2;
                        radius: 16;
                    }

                    Image {
                        visible: true;
                        id: gridItemImage;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            bottom: parent.bottom;
                        }

                        fillMode: Image.PreserveAspectFit;
                        onStatusChanged: {
                            if ( status == Image.Null || status === Image.Error) {
                                console.log("Error in " + source)
                            }
                        }

                        height: parent.height;
                        asynchronous: true;
                        source: imageCacher.cachedUrl;
                        verticalAlignment: Image.AlignBottom;
                        horizontalAlignment: Image.AlignLeft;

                        sourceSize {
                            height: 400;
                            width: 400;
                        }

                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                bladeListView.currentIndex = index;
                            }
                        }

                        ImageCacher {
                            id: imageCacher;

                            imageUrl: artworkUrl;
                            identifier: sha1;

                            Component.onCompleted: {
                                cache();
                            }
                        }
                    }

                    Text {
                        id: titleText;

                        anchors {
                            top: gridItemImage.bottom;
                            topMargin: 6;
                        }
                        width: gridItemImage.width;

                        text: title;
                        color: index !== bladeListView.currentIndex ? "darkgray" : "white";
                        elide: Text.ElideRight;
                    }
                }
            }
        }

        ListView {
            id: repeater;
            anchors.fill: parent;
            verticalLayoutDirection: ListView.BottomToTop;

            function changePosition( event ) {

                if ( event === InputDeviceEvent.Down) {

                    repeater.decrementCurrentIndex();
                    repeater.currentItem.checked = true;

                } else if ( event === InputDeviceEvent.Up ) {

                    if ( repeater.currentIndex === count - 1 ) {
                        repeater.currentIndex.checked = false;
                        repeater.focus = false;
                        sectionRow.focus = true;
                        return;
                    }

                    repeater.incrementCurrentIndex();

                } else if ( event === InputDeviceEvent.Left ) {

                    repeater.currentItem.displayView.decrementCurrentIndex();

                } else if ( event === InputDeviceEvent.Right ) {

                    repeater.currentItem.displayView.incrementCurrentIndex();

                }

                repeater.currentItem.checked = true;

            }

            onFocusChanged: {
                currentItem.checked = focus;
            }

            currentIndex: count - 1;

            model: ListModel {
                ListElement { bgColor: "#ff5f5f"; heading: "All"; selected: false; }
                ListElement { bgColor: "#8155f1"; heading: "Recently Added"; selected: false; }
                ListElement { bgColor: "yellow"; heading: "Collections"; selected: false; }
            }

            ExclusiveGroup {
                id: bladeGroup;
            }

            property int collapsedHeight: 50;
            property int maxHeight: 300;

            delegate: Item {
                id: bladeItem;
                height: checked ? repeater.maxHeight : repeater.collapsedHeight;
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                z: -index;

                property bool checked: selected;
                property ExclusiveGroup exclusiveGroup: bladeGroup;
                property var displayView: null;



                onExclusiveGroupChanged: {
                    if ( exclusiveGroup ) {
                        exclusiveGroup.bindCheckable( bladeItem );
                    }
                }

                onHeightChanged: {
                    if ( checked && height === repeater.maxHeight ) {
                        bladeItem.displayView = listView.createObject( innerBladeArea );
                    }
                }

                Behavior on height {
                    PropertyAnimation { id: heightAnimation; duration: 200 }
                }

                Rectangle {
                    anchors {
                        bottom: blade.top;
                        left: parent.left;
                        right: parent.right;
                    }

                    height: 5;
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "transparent"; }
                        GradientStop { position: 0.05; color: "transparent"; }
                        GradientStop { position: 1.0; color: "black"; }
                    }
                }

                Rectangle {
                    id: blade;
                    color: bgColor;
                    anchors.fill: parent;

                    Rectangle {
                        id: topBladeAccent;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top: parent.top;
                        }
                        height: 2;
                        color: "white";
                        opacity: 0.25;
                    }

                    Rectangle {
                        anchors {
                            left: parent.left;
                            top: topBladeAccent.bottom;
                            bottom: parent.bottom;
                        }
                        width: 2;
                        color: "white";
                        opacity: 0.15;
                    }

                    Rectangle {
                        anchors {
                            right: parent.right;
                            top: topBladeAccent.bottom;
                            bottom: parent.bottom;
                        }
                        width: 2;
                        color: "white";
                        opacity: 0.15;
                    }

                    Text {
                        id: headingText;
                        anchors {
                            left: parent.left;
                            leftMargin: 12;
                            top: parent.top;
                            topMargin: repeater.collapsedHeight / 4;
                        }

                        text: qsTr( heading );
                        color: "white";
                        opacity: 0.75;
                        font {
                            bold: true;
                            pixelSize: 16;
                        }
                    }

                    Rectangle {
                        id: innerBladeArea;
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                            bottom: parent.bottom;
                            topMargin: repeater.collapsedHeight;
                        }
                        color: "transparent";

                        property bool checked: bladeItem.checked;

                        Connections {
                            target: bladeItem;
                            onCheckedChanged: {
                                if ( !checked ) {
                                    if ( bladeItem.displayView ) {
                                        bladeItem.displayView.destroy();
                                    }
                                }
                            }

                        }

                    }

                    MouseArea {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                        }

                        height: repeater.collapsedHeight;
                        onClicked: {
                            repeater.currentIndex = index;

                            if ( bladeItem.checked ) {
                                bladeItem.checked = false;
                            } else {
                                bladeItem.checked = true;
                            }
                        }
                    }
                }
            }
        }
    }

    /*
    Rectangle {
        anchors {
            top: bigPictureViewColumn.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }

            id: bladeContainer;
            property ExclusiveGroup gamesBladeGroup: ExclusiveGroup {}

            Item {
                id: bladeItem;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                }
                height:  300//checked ? 300 : 45;

                property ExclusiveGroup exclusiveGroup: bladeContainer.gamesBladeGroup;
                property bool checked:  false;

                onExclusiveGroupChanged: {
                    if ( exclusiveGroup ) {
                        exclusiveGroup.bindCheckable( bladeItem );
                    }
                }

                DropShadow {
                    anchors.fill: source;
                    source: blade;
                    verticalOffset: -3;
                    horizontalOffset: 0;
                    color: "black";
                    transparentBorder: true;
                    radius: 4;
                    samples: radius * 2;
                    cached: true;
                }

                Rectangle {
                    id: blade;
                    anchors.fill: parent;
                    color: "orange";



                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            bladeItem.checked = true;
                        }
                    }

                    Behavior on height {
                        PropertyAnimation {
                            duration: 100;
                            easing.type: Easing.Bezier;
                        }
                    }
                }
            }
        }

        Item {
            id: bladeItem2;
            anchors {
                left: parent.left;
                right: parent.right;
                top: bladeItem.top;
                topMargin: 25;
            }
            height:  300//checked ? 300 : 45;

            property ExclusiveGroup exclusiveGroup: bladeContainer.gamesBladeGroup;
            property bool checked:  true;
            onExclusiveGroupChanged: {
                if ( exclusiveGroup ) {
                    exclusiveGroup.bindCheckable( bladeItem );
                }
            }
            z: blade.z + 1 ;
            y: bladeItem.y - 100;

            DropShadow {
                anchors.fill: source;
                source: blade2;
                verticalOffset: -3;
                horizontalOffset: 0;
                color: "black";
                transparentBorder: true;
                radius: 4;
                samples: radius * 2;
                cached: true;
            }

            Rectangle {
                id: blade2;
                anchors.fill: parent;
                color: "yellow";

                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        bladeItem.checked = true;
                    }
                }

                Behavior on height {
                    PropertyAnimation {
                        duration: 100;
                        easing.type: Easing.Bezier;
                    }
                }
            }
        }
        */

}
