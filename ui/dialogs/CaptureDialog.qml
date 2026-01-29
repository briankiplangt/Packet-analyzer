// CaptureDialog.qml - Network interface selection and capture settings
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: captureDialog
    title: "Start Packet Capture"
    width: 400
    height: 300
    modal: true
    
    // Signals
    signal captureStarted(string interfaceName, string filter)
    
    background: Rectangle {
        color: "#1a1a1a"
        radius: 8
        border.color: "#404040"
        border.width: 1
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Title
        Text {
            text: "📡 Configure Packet Capture"
            color: "#66d9ff"
            font.pixelSize: 16
            font.weight: Font.Bold
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Interface selection
        GroupBox {
            title: "Network Interface"
            Layout.fillWidth: true
            
            background: Rectangle {
                color: "#0d0d0d"
                radius: 6
                border.color: "#404040"
                border.width: 1
            }
            
            label: Text {
                text: parent.title
                color: "#ffffff"
                font.pixelSize: 12
                font.weight: Font.Bold
            }
            
            ComboBox {
                id: interfaceCombo
                width: parent.width
                model: ["eth0 (Ethernet)", "wlan0 (WiFi)", "lo (Loopback)"]
                currentIndex: 0
                
                background: Rectangle {
                    color: "#2d2d2d"
                    radius: 4
                    border.color: "#404040"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: interfaceCombo.displayText
                    color: "#ffffff"
                    font.pixelSize: 11
                    verticalAlignment: Text.AlignVCenter
                    leftPadding: 10
                }
            }
        }
        
        // Filter settings
        GroupBox {
            title: "Capture Filter (BPF)"
            Layout.fillWidth: true
            
            background: Rectangle {
                color: "#0d0d0d"
                radius: 6
                border.color: "#404040"
                border.width: 1
            }
            
            label: Text {
                text: parent.title
                color: "#ffffff"
                font.pixelSize: 12
                font.weight: Font.Bold
            }
            
            Column {
                width: parent.width
                spacing: 8
                
                TextField {
                    id: filterField
                    width: parent.width
                    placeholderText: "e.g., tcp port 80, udp, icmp"
                    text: ""
                    
                    background: Rectangle {
                        color: "#2d2d2d"
                        radius: 4
                        border.color: "#404040"
                        border.width: 1
                    }
                    
                    color: "#ffffff"
                    font.pixelSize: 11
                    font.family: "Consolas, Monaco, monospace"
                }
                
                Row {
                    spacing: 10
                    
                    Button {
                        text: "HTTP"
                        onClicked: filterField.text = "tcp port 80 or tcp port 443"
                        
                        background: Rectangle {
                            color: parent.pressed ? "#4a4a4a" : "#3a3a3a"
                            radius: 4
                            border.color: "#666666"
                            border.width: 1
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    
                    Button {
                        text: "DNS"
                        onClicked: filterField.text = "udp port 53"
                        
                        background: Rectangle {
                            color: parent.pressed ? "#4a4a4a" : "#3a3a3a"
                            radius: 4
                            border.color: "#666666"
                            border.width: 1
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    
                    Button {
                        text: "Clear"
                        onClicked: filterField.text = ""
                        
                        background: Rectangle {
                            color: parent.pressed ? "#4a4a4a" : "#3a3a3a"
                            radius: 4
                            border.color: "#666666"
                            border.width: 1
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: "#ffffff"
                            font.pixelSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }
        }
        
        Item { Layout.fillHeight: true } // Spacer
        
        // Buttons
        Row {
            Layout.alignment: Qt.AlignHCenter
            spacing: 15
            
            Button {
                text: "Start Capture"
                width: 120
                height: 35
                
                background: Rectangle {
                    color: parent.pressed ? "#1e5a3e" : "#28a745"
                    radius: 6
                    border.color: "#00ff88"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.weight: Font.Bold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    var interfaceName = interfaceCombo.currentText.split(" ")[0]
                    captureStarted(interfaceName, filterField.text)
                    captureDialog.close()
                }
            }
            
            Button {
                text: "Cancel"
                width: 120
                height: 35
                
                background: Rectangle {
                    color: parent.pressed ? "#5a3e3e" : "#6c757d"
                    radius: 6
                    border.color: "#999999"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: captureDialog.close()
            }
        }
    }
}