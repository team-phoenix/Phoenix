/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
   PhxScrollView extends the scrollbar example by having the scrollbar handles automatically show on mouseover
   in addition to showing while scrolling. The handles will be visible for a minimum of 2 seconds once made to
   show thanks to the addition of a Timer.

   In order to make PhxScrollView reusable, a Flickable property is exposed which, by default, is assigned to the last
   child of this object. In practice, this means that all you need to do is make sure the last child of your
   PhxScrollView is a Flickable.
*/

import QtQuick 2.5

FocusScope {
    property int scrollBarScale: 12;
    property Flickable flickable: children[ children.length - 1 ];

    // Only show the scrollbars when conditions are met
    states: State {
        name: "ShowBars";
        when: flickable.movingVertically || flickable.movingHorizontally ||
              flickable.verticalVelocity !== 0.0 || flickable.horizontalVelocity !== 0.0 ||
              verticalScrollBar.backgroundMousedOver || horizontalScrollBar.backgroundMousedOver ||
              scrollBarHideTimer.running;
        PropertyChanges { target: verticalScrollBar; opacity: 1; }
        PropertyChanges { target: horizontalScrollBar; opacity: 1; }
        PropertyChanges { target: scrollBarHideTimer; running: true; }
    }

    // Ensures scrollbars will always be visible for at least 2 seconds at a time
    Timer {
        id: scrollBarHideTimer;
        interval: 2000;
        running: false;
    }

    transitions: Transition {
        NumberAnimation { properties: "opacity"; duration: 250; }
    }

    // Attach scrollbars to the right and bottom edges of the flickable
    PhxScrollBar {
        id: verticalScrollBar;
        width: scrollBarScale * 2; height: parent.height - ( horizontalScrollBar.visible ? scrollBarScale : 0 );
        z: flickable.z + 1;
        visible: flickable.contentHeight > parent.height;
        enabled: visible;
        handleWidth: scrollBarScale;
        anchors.right: parent.right;
        opacity: 0;
        orientation: Qt.Vertical;
        position: isNaN( flickable.visibleArea.yPosition ) ? 0 : flickable.visibleArea.yPosition;
        pageSize: isNaN( flickable.visibleArea.heightRatio ) ? 0 : flickable.visibleArea.heightRatio;
    }

    PhxScrollBar {
        id: horizontalScrollBar;
        width: parent.width - ( verticalScrollBar.visible ? scrollBarScale : 0 ); height: scrollBarScale * 2;
        z: flickable.z + 1;
        visible: flickable.contentWidth > parent.width;
        enabled: visible;
        handleHeight: scrollBarScale;
        anchors.bottom: parent.bottom;
        opacity: 0;
        orientation: Qt.Horizontal;
        position: isNaN( flickable.visibleArea.xPosition ) ? 0 : flickable.visibleArea.xPosition;
        pageSize: isNaN( flickable.visibleArea.widthRatio ) ? 0 : flickable.visibleArea.widthRatio;
    }
}
