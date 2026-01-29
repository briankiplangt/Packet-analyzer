// MenuBarButton.qml - Reusable menu bar button component
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: menuButton
    
    // Public properties
    property string text: "Button"
    property string baseColor: "#1e3a5f"
    property string hoverColor: "#2a4a6a"
    property string glowColor: "#66d9ff"
    property bool isActive: false
    
    // Signal for click events
    signal clicked()
    
    // Button appearance
    width: 120
    height: 36
    radius: 18
    color: buttonHover.containsMouse ? hoverColor : baseColor
    border.color: isActive ? glowColor : "#404040"
    border.width: 2
    
    // Glow effect when active
    opacity: isActive ? 1.0 : 0.8
    
    // Button content
    Row {
        anchors.centerIn: parent
        spacing: 8
        
        Text {
            text: menuButton.text
            color: "#FFFFFF"
            font.pixelSize: 13
            font.weight: Font.Medium
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    // Mouse interaction
    MouseArea {
        id: buttonHover
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: menuButton.clicked()
    }
    
    // Smooth animations
    Behavior on color { ColorAnimation { duration: 200 } }
    Behavior on scale { NumberAnimation { duration: 100 } }
    Behavior on opacity { NumberAnimation { duration: 200 } }
    
    // Press animation
    scale: buttonHover.pressed ? 0.95 : 1.0
}