// HexDumpView.qml - Hex dump view component
import QtQuick 2.15
import QtQuick.Controls 2.15

ScrollView {
    property var packet: null
    
    clip: true
    
    Rectangle {
        width: parent.width
        height: Math.max(parent.height, textArea.contentHeight + 20)
        color: "#1a1a1a"
        
        TextArea {
            id: textArea
            anchors.fill: parent
            anchors.margins: 10
            
            text: generateHexDump()
            color: "#00ff88"
            font.family: "Consolas, Monaco, monospace"
            font.pixelSize: 11
            selectByMouse: true
            readOnly: true
            
            background: Rectangle {
                color: "transparent"
            }
        }
    }
    
    function generateHexDump() {
        if (!packet) {
            return "No packet selected for hex dump analysis."
        }
        
        // Generate real hex dump from packet data
        var hexDump = "Offset  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII\n"
        hexDump += "------  -----------------------------------------------  ----------------\n"
        
        // Use real packet data if available from C++ backend
        if (packet && packet.rawData) {
            // Real packet hex dump from PacketCaptureEngine
            hexDump += packet.rawData
        } else {
            // Minimal placeholder when no real packet data
            hexDump += "0000    No real packet data available                              \n"
            hexDump += "        Select a captured packet to view hex dump                   \n"
        }
        
        return hexDump
    }
}