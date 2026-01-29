import QtQuick 2.15

Rectangle {
    property string title: ""
    property alias content: contentArea.children
    
    color: "#333333"
    radius: 8
    border.color: "#555555"
    border.width: 1
    
    // Glassmorphism effect
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: "transparent"
        border.color: "#ffffff10"
        border.width: 1
    }
    
    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 8
        
        // Title
        Text {
            text: title
            color: "#ffffff"
            font.pixelSize: 12
            font.bold: true
            width: parent.width
        }
        
        // Content area
        Item {
            id: contentArea
            width: parent.width
            height: parent.height - 20
        }
    }
    
    // Subtle hover effect
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            parent.border.color = "#666666"
        }
        
        onExited: {
            parent.border.color = "#555555"
        }
    }
    
    Behavior on border.color {
        ColorAnimation { duration: 200 }
    }
}