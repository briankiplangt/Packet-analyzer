// GlassCard.qml - Glassmorphism card component
import QtQuick 2.15

Rectangle {
    id: root
    
    color: Qt.rgba(0.2, 0.2, 0.2, 0.8)
    radius: 12
    border.color: Qt.rgba(1, 1, 1, 0.1)
    border.width: 1
    
    // Glassmorphism backdrop blur effect
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(1, 1, 1, 0.05)
        radius: parent.radius
        
        // Inner glow
        Rectangle {
            anchors.fill: parent
            anchors.margins: 1
            color: "transparent"
            radius: parent.radius - 1
            border.color: Qt.rgba(1, 1, 1, 0.1)
            border.width: 1
        }
    }
    
    // Subtle shadow effect
    Rectangle {
        anchors.fill: parent
        anchors.topMargin: 2
        anchors.leftMargin: 2
        color: Qt.rgba(0, 0, 0, 0.1)
        radius: parent.radius
        z: -1
    }
}