// AsciiView.qml - ASCII packet data interpretation
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: asciiView
    color: "#0d0d0d"
    
    property var hexDumpLines: []
    property string selectedPacketData: ""
    
    Column {
        anchors.fill: parent
        
        // Header
        Rectangle {
            width: parent.width
            height: 35
            color: "#1a1a1a"
            border.color: "#404040"
            border.width: 1
            
            Row {
                anchors.centerIn: parent
                spacing: 8
                
                Text {
                    text: "📝"
                    font.pixelSize: 16
                    color: "#66d9ff"
                }
                
                Text {
                    text: "ASCII View"
                    color: "#66d9ff"
                    font.pixelSize: 13
                    font.weight: Font.Bold
                }
            }
        }
        
        // ASCII content
        ScrollView {
            width: parent.width
            height: parent.height - 35
            clip: true
            
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                contentItem: Rectangle {
                    implicitWidth: 8
                    radius: 4
                    color: parent.pressed ? "#66d9ff" : "#404040"
                    Behavior on color { ColorAnimation { duration: 200 } }
                }
            }
            
            Column {
                width: parent.width
                padding: 10
                spacing: 2
                
                // ASCII lines from hex dump
                Repeater {
                    model: hexDumpLines
                    
                    Rectangle {
                        width: parent.width - 20
                        height: 20
                        color: asciiLineHover.containsMouse ? "#1a1a1a" : "transparent"
                        radius: 3
                        
                        Row {
                            anchors.fill: parent
                            spacing: 10
                            
                            // Offset
                            Text {
                                text: modelData.offset
                                color: "#999999"
                                font.pixelSize: 11
                                font.family: "Consolas, Monaco, 'Courier New', monospace"
                                width: 40
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            // ASCII interpretation
                            Text {
                                text: modelData.ascii
                                color: "#ffffff"
                                font.pixelSize: 11
                                font.family: "Consolas, Monaco, 'Courier New', monospace"
                                anchors.verticalCenter: parent.verticalCenter
                                
                                // Highlight readable characters
                                Component.onCompleted: {
                                    // Color readable ASCII green, non-readable gray
                                    var readable = /^[!-~\s]*$/.test(modelData.ascii)
                                    color = readable ? "#00ff88" : "#666666"
                                }
                            }
                        }
                        
                        MouseArea {
                            id: asciiLineHover
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                        }
                        
                        Behavior on color { ColorAnimation { duration: 100 } }
                    }
                }
                
                // Decoded content section
                Rectangle {
                    width: parent.width - 20
                    height: Math.max(60, decodedContent.implicitHeight + 20)
                    color: "#1a1a1a"
                    radius: 8
                    border.color: "#404040"
                    border.width: 1
                    visible: selectedPacketData.length > 0
                    
                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 8
                        
                        Text {
                            text: "🔍 Decoded Content:"
                            color: "#66d9ff"
                            font.pixelSize: 12
                            font.weight: Font.Bold
                        }
                        
                        ScrollView {
                            width: parent.width
                            height: parent.height - 30
                            clip: true
                            
                            Text {
                                id: decodedContent
                                width: parent.width - 10
                                text: extractReadableText(selectedPacketData)
                                color: "#00ff88"
                                font.pixelSize: 11
                                font.family: "Consolas, Monaco, 'Courier New', monospace"
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Function to extract readable text from packet data
    function extractReadableText(data) {
        if (!data || data.length === 0) {
            return "No packet selected"
        }
        
        // Extract common readable patterns
        var patterns = [
            "Host: ",
            "User-Agent: ",
            "GET ",
            "POST ",
            "HTTP/",
            "Content-Type: ",
            "google.com",
            "cloudflare.com",
            "mozilla.org"
        ]
        
        var result = ""
        for (var i = 0; i < patterns.length; i++) {
            if (data.indexOf(patterns[i]) !== -1) {
                result += patterns[i] + "\n"
            }
        }
        
        return result || "Binary data - no readable text found"
    }
    
    // Update ASCII view when packet selection changes
    function updatePacketData(packetData) {
        selectedPacketData = packetData || ""
    }
}