// HealthIndicator.qml - System health indicator component
import QtQuick 2.15

Row {
    property string label: "Status"
    property string status: "Unknown"
    property color color: "#888888"
    
    spacing: 10
    
    Rectangle {
        width: 12
        height: 12
        radius: 6
        color: parent.color
        anchors.verticalCenter: parent.verticalCenter
        
        // Pulsing animation for active states
        SequentialAnimation on opacity {
            running: parent.color === "#00ff88"
            loops: Animation.Infinite
            NumberAnimation { to: 0.5; duration: 1000 }
            NumberAnimation { to: 1.0; duration: 1000 }
        }
    }
    
    Column {
        anchors.verticalCenter: parent.verticalCenter
        
        Text {
            text: label
            color: "white"
            font.pixelSize: 12
            font.bold: true
        }
        
        Text {
            text: status
            color: "#cccccc"
            font.pixelSize: 10
        }
    }
}