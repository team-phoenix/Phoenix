import QtQuick 2.0
import QtGraphicalEffects 1.0

import vg.phoenix.backend 1.0
import vg.phoenix.themes 1.0
import vg.phoenix.launcher 1.0
import vg.phoenix.paths 1.0

// A cool logo effect
Item {
    id: logoContainer;
    property alias phoenixLogo: phoenixLogo;
    property alias phoenixLogoAnimation: phoenixLogoAnimation;

    property int animationSpeed: 250;

    // Glow effect for the logo
    // Only visible when a game is not running
    FastBlur {
        id: phoenixLogoBlur;
        anchors.fill: phoenixLogo;
        source: phoenixLogo;
        scale: phoenixLogo.scale;
        transparentBorder: true;
        opacity: phoenixLogo.opacity;
        radius: 64;
    }

    // A logo
    // Only visible when a game is not running
    Image {
        id: phoenixLogo;
        anchors.centerIn: parent;
        width: parent.width;
        height: parent.height;
        source: "../phoenix.png";
        opacity: 1.0;
        sourceSize { height: height; width: width; }
    }

    ParallelAnimation {
        id: phoenixLogoAnimation;
        SequentialAnimation {
            ScriptAction { script: phoenixLogoBlurAnimation.stop(); }
            ParallelAnimation {
                PropertyAction { target: phoenixLogoBlur; property: "radius"; value: 64; }
                PropertyAnimation { target: logoContainer; property: "scale"; from: 1.0; to: 0.75; duration: logoContainer.animationSpeed; }
            }
            ScriptAction { script: phoenixLogoBlurAnimation.restart(); }
        }
    }

    SequentialAnimation {
        id: phoenixLogoBlurAnimation;
        loops: Animation.Infinite;
        PropertyAnimation { target: phoenixLogoBlur; property: "radius"; to: 64; duration: 2000; easing.type: Easing.InOutSine; }
        PropertyAnimation { target: phoenixLogoBlur; property: "radius"; to: 32; duration: 2000; easing.type: Easing.InOutSine; }
    }
}
