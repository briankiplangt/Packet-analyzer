// BandwidthChart.qml - Live bandwidth visualization using Canvas
import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: chartContainer
    color: "#0d0d0d"
    radius: 12
    border.color: "#404040"
    border.width: 1
    
    // Public properties
    property bool isCapturing: false
    property real currentBandwidth: 0.0
    property int maxDataPoints: 50
    property real maxBandwidth: 200.0
    
    // Internal data
    property var dataPoints: []
    
    Column {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // Header
        Row {
            width: parent.width
            spacing: 10
            
            Text {
                text: "📊"
                font.pixelSize: 20
                color: "#66d9ff"
            }
            
            Text {
                text: "Bandwidth Monitor"
                color: "#ffffff"
                font.pixelSize: 14
                font.weight: Font.Bold
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Rectangle {
                width: 60
                height: 20
                radius: 10
                color: isCapturing ? "#28a74530" : "#4d1a1a30"
                border.color: isCapturing ? "#28a745" : "#ff6b6b"
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter
                
                Text {
                    anchors.centerIn: parent
                    text: isCapturing ? "LIVE" : "STOP"
                    color: isCapturing ? "#28a745" : "#ff6b6b"
                    font.pixelSize: 9
                    font.weight: Font.Bold
                }
                
                SequentialAnimation on opacity {
                    running: isCapturing
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.5; duration: 800 }
                    NumberAnimation { to: 1.0; duration: 800 }
                }
            }
        }
        
        // Canvas chart
        Canvas {
            id: bandwidthChart
            width: parent.width
            height: 130
            
            // Real bandwidth data comes from PacketCaptureEngine signals
            // No Timer needed - data updates via onBandwidthChanged signal
            
            function addRealDataPoint(bandwidth) {
                dataPoints.push(bandwidth)
                
                // Keep only last N points
                if (dataPoints.length > maxDataPoints) {
                    dataPoints.shift()
                }
                
                bandwidthChart.requestPaint()
            }
            
            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)
                
                if (dataPoints.length < 2) return
                
                // Draw grid lines
                ctx.strokeStyle = "#2d2d2d"
                ctx.lineWidth = 1
                
                for (var i = 0; i <= 4; i++) {
                    var y = (height / 4) * i
                    ctx.beginPath()
                    ctx.moveTo(0, y)
                    ctx.lineTo(width, y)
                    ctx.stroke()
                }
                
                // Draw Y-axis labels
                ctx.fillStyle = "#999999"
                ctx.font = "10px Consolas"
                ctx.fillText(maxBandwidth + " MB/s", 5, 15)
                ctx.fillText((maxBandwidth * 0.75) + " MB/s", 5, height * 0.25 + 5)
                ctx.fillText((maxBandwidth * 0.5) + " MB/s", 5, height * 0.5 + 5)
                ctx.fillText((maxBandwidth * 0.25) + " MB/s", 5, height * 0.75 + 5)
                ctx.fillText("0", 5, height - 5)
                
                // Draw animated line chart
                var gradient = ctx.createLinearGradient(0, 0, 0, height)
                gradient.addColorStop(0, "#00ff88")
                gradient.addColorStop(0.5, "#66d9ff")
                gradient.addColorStop(1, "#ff66b3")
                
                ctx.strokeStyle = gradient
                ctx.lineWidth = 3
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                
                ctx.beginPath()
                
                var pointWidth = width / maxDataPoints
                
                for (var i = 0; i < dataPoints.length; i++) {
                    var x = i * pointWidth
                    var y = height - (dataPoints[i] / maxBandwidth * height)
                    
                    if (i === 0) {
                        ctx.moveTo(x, y)
                    } else {
                        ctx.lineTo(x, y)
                    }
                }
                
                ctx.stroke()
                
                // Draw fill area
                if (dataPoints.length > 0) {
                    ctx.lineTo(width, height)
                    ctx.lineTo(0, height)
                    ctx.closePath()
                    
                    var fillGradient = ctx.createLinearGradient(0, 0, 0, height)
                    fillGradient.addColorStop(0, "#00ff8840")
                    fillGradient.addColorStop(1, "#00ff8810")
                    ctx.fillStyle = fillGradient
                    ctx.fill()
                }
                
                // Draw data points
                for (var i = 0; i < dataPoints.length; i++) {
                    var x = i * pointWidth
                    var y = height - (dataPoints[i] / maxBandwidth * height)
                    
                    // Outer glow
                    ctx.beginPath()
                    ctx.arc(x, y, 4, 0, 2 * Math.PI)
                    ctx.fillStyle = "#00ff8840"
                    ctx.fill()
                    
                    // Inner dot
                    ctx.beginPath()
                    ctx.arc(x, y, 2, 0, 2 * Math.PI)
                    ctx.fillStyle = "#00ff88"
                    ctx.fill()
                }
            }
        }
    }
    
    // Initialize with empty data - real data comes from backend
    Component.onCompleted: {
        // Start with empty array - real bandwidth data will populate this
        dataPoints = []
    }
    
    // Connect to real bandwidth updates
    onCurrentBandwidthChanged: {
        addRealDataPoint(currentBandwidth)
    }
}