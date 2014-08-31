import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

PhoenixNormalButton {
    buttonGradient: Gradient {
        GradientStop {position: 0.0; color: pressed ? "#5c201e" : "#e85955";}
        GradientStop {position: 1.0; color: pressed ? "#9e3b38" : "#e8433f";}
    }

    innerBorderGradient: Gradient {
        GradientStop {position: 0.0; color: pressed ? "#4d1b19" : "#db5753";}
        GradientStop {position: 1.0; color: pressed ? "#5c201e" : "#a22f2c";}
    }
}
