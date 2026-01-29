import QtQuick 2.15

Rectangle {
    id: packetDelegate
    
    property string number: ""
    property string time: ""
    property string source: ""
    property string destination: ""
    property string protocol: ""
    property string length: ""
    property string info: ""
    property color protocolColor: "#4488ff"
    
    width: parent.width
    height: 25
    color: mouseArea.containsMouse ? "#444444" : (index % 2 === 0 ? "#2a2a2a" : "#333333")
    
    // Smooth hover transition
    Behavior on color {
        ColorAnimation { duration: 150 }
    }
    
    // Protocol color indicator
    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 3
        color: protocolColor
    }
    
    Row {
        anchors.fill: parent
        anchors.margins: 5
        anchors.leftMargin: 8
        
        Text {
            text: number
            color: "#cccccc"
            width: 50
            font.pixelSize: 11
            font.family: "Consolas, Monaco, monospace"
            verticalAlignment: Text.AlignVCenter
        }
        
        Text {
            text: time
            color: "#aaaaaa"
            width: 100
            font.pixelSize: 11
            font.family: "Consolas, Monaco, monospace"
            verticalAlignment: Text.AlignVCenter
        }
        
        Text {
            text: source
            color: "#ffffff"
            width: 150
            font.pixelSize: 11
            font.family: "Consolas, Monaco, monospace"
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        
        Text {
            text: destination
            color: "#ffffff"
            width: 150
            font.pixelSize: 11
            font.family: "Consolas, Monaco, monospace"
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        
        Rectangle {
            width: 80
            height: 18
            radius: 9
            color: protocolColor
            anchors.verticalCenter: parent.verticalCenter
            
            Text {
                text: protocol
                color: "#ffffff"
                font.pixelSize: 10
                font.bold: true
                anchors.centerIn: parent
            }
        }
        
        Text {
            text: length
            color: "#cccccc"
            width: 80
            font.pixelSize: 11
            font.family: "Consolas, Monaco, monospace"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignRight
        }
        
        Text {
            text: info
            color: "#dddddd"
            width: 200
            font.pixelSize: 11
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        
        onClicked: {
            console.log("Selected packet:", number)
            // Update packet details view
        }
        
        onDoubleClicked: {
            console.log("Double-clicked packet:", number)
            // Open detailed packet analysis
        }
    }
    
    // Selection highlight
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: mouseArea.pressed ? protocolColor : "transparent"
        border.width: 2
        radius: 2
        
        Behavior on border.color {
            ColorAnimation { duration: 100 }
        }
    }
}