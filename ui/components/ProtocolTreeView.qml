// ProtocolTreeView.qml - Protocol tree view component
import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollView {
    property var packet: null
    
    clip: true
    
    Rectangle {
        width: parent.width
        height: Math.max(parent.height, column.height)
        color: "#1a1a1a"
        
        Column {
            id: column
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5
            
            Text {
                text: packet ? "📋 Protocol Analysis" : "📋 No packet selected"
                color: "white"
                font.bold: true
                font.pixelSize: 14
            }
            
            Rectangle {
                width: parent.width
                height: 1
                color: "#333333"
            }
            
            // Ethernet frame
            Text {
                text: "🔗 Ethernet II"
                color: "#88ccff"
                font.bold: true
                font.pixelSize: 12
            }
            
            Text {
                text: packet ? `   Source: ${packet.sourceIP}` : "   Source: N/A"
                color: "#cccccc"
                font.pixelSize: 11
                leftPadding: 20
            }
            
            Text {
                text: packet ? `   Destination: ${packet.destIP}` : "   Destination: N/A"
                color: "#cccccc"
                font.pixelSize: 11
                leftPadding: 20
            }
            
            // IP layer
            Text {
                text: "🌐 Internet Protocol Version 4 (IPv4)"
                color: "#88ff88"
                font.bold: true
                font.pixelSize: 12
            }
            
            Text {
                text: packet ? `   Protocol: ${packet.protocol}` : "   Protocol: N/A"
                color: "#cccccc"
                font.pixelSize: 11
                leftPadding: 20
            }
            
            Text {
                text: packet ? `   Length: ${packet.size} bytes` : "   Length: N/A"
                color: "#cccccc"
                font.pixelSize: 11
                leftPadding: 20
            }
            
            // Transport layer
            Text {
                text: packet && packet.protocol === "TCP" ? "🚛 Transmission Control Protocol (TCP)" :
                      packet && packet.protocol === "UDP" ? "📦 User Datagram Protocol (UDP)" :
                      "🔄 Transport Layer"
                color: "#ffcc88"
                font.bold: true
                font.pixelSize: 12
            }
            
            Text {
                text: packet && packet.flags ? `   Flags: ${packet.flags}` : "   Flags: N/A"
                color: "#cccccc"
                font.pixelSize: 11
                leftPadding: 20
            }
            
            // Application layer
            Text {
                text: packet && packet.info ? `📱 Application: ${packet.info}` : "📱 Application Layer"
                color: "#ff88cc"
                font.bold: true
                font.pixelSize: 12
            }
        }
    }
}