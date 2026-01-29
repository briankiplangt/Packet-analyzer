// ModernButton.qml - Modern styled button component
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    
    property color color: "#00ff88"
    property color hoverColor: Qt.lighter(color, 1.2)
    property color pressColor: Qt.darker(color, 1.2)
    
    width: 120
    height: 40
    
    background: Rectangle {
        color: control.pressed ? pressColor : 
               (control.hovered ? hoverColor : control.color)
        radius: 8
        
        Behavior on color {
            ColorAnimation { duration: 150 }
        }
        
        // Glassmorphism effect
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: Qt.rgba(1, 1, 1, 0.2)
            border.width: 1
            radius: parent.radius
        }
    }
    
    contentItem: Text {
        text: control.text
        font.bold: true
        font.pixelSize: 14
        color: "#1a1a1a"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
    
    // Hover animation
    scale: control.pressed ? 0.95 : (control.hovered ? 1.05 : 1.0)
    
    Behavior on scale {
        NumberAnimation { duration: 100 }
    }
}