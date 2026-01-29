// NetworkTopologyMap.qml - Live Network Topology Visualization
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 1.15

Rectangle {
    id: topologyMap
    color: "#0d0d0d"
    border.color: "#404040"
    border.width: 1
    radius: 8
    
    property var networkNodes: []
    property var networkConnections: []
    property bool isCapturing: false
    property var activeConnections: []
    
    // Network discovery timer
    Timer {
        id: discoveryTimer
        interval: 2000
        running: isCapturing
        repeat: true
        onTriggered: updateNetworkTopology()
    }
    
    // Connection animation timer
    Timer {
        id: connectionTimer
        interval: 100
        running: isCapturing
        repeat: true
        onTriggered: animateConnections()
    }
    
    // Initialize with local network
    Component.onCompleted: {
        initializeNetwork()
    }
    
    function initializeNetwork() {
        // Add local machine
        var localNode = {
            id: "local",
            ip: "192.168.1.100",
            type: "local",
            x: width * 0.5,
            y: height * 0.5,
            connections: 0,
            lastSeen: Date.now(),
            color: "#00ff88"
        }
        
        // Add common network devices
        var nodes = [
            localNode,
            {
                id: "router",
                ip: "192.168.1.1",
                type: "router",
                x: width * 0.2,
                y: height * 0.3,
                connections: 0,
                lastSeen: Date.now(),
                color: "#66d9ff"
            },
            {
                id: "dns1",
                ip: "8.8.8.8",
                type: "server",
                x: width * 0.8,
                y: height * 0.2,
                connections: 0,
                lastSeen: Date.now(),
                color: "#ffc107"
            },
            {
                id: "dns2",
                ip: "1.1.1.1",
                type: "server",
                x: width * 0.8,
                y: height * 0.8,
                connections: 0,
                lastSeen: Date.now(),
                color: "#ffc107"
            },
            {
                id: "web1",
                ip: "142.250.1.1",
                type: "server",
                x: width * 0.1,
                y: height * 0.7,
                connections: 0,
                lastSeen: Date.now(),
                color: "#ff66b3"
            }
        ]
        
        networkNodes = nodes
        networkNodesChanged()
    }
    
    function updateNetworkTopology() {
        // Real network discovery - update existing nodes only
        for (var i = 0; i < networkNodes.length; i++) {
            var node = networkNodes[i]
            // Real network activity will be provided by PacketCaptureEngine
            // This function now only updates display, no simulation
        }
        
        networkNodesChanged()
    }
    
    function addRandomNode() {
        // Real nodes will be added via addPacketFlow() when actual packets are captured
        // No random node generation - only real network nodes from packet analysis
        console.log("🌐 Real network nodes will be added from actual packet capture")
    }
    
    function getNodeById(id) {
        for (var i = 0; i < networkNodes.length; i++) {
            if (networkNodes[i].id === id) {
                return networkNodes[i]
            }
        }
        return null
    }
    
    function addConnectionAnimation(fromNode, toNode) {
        if (!fromNode || !toNode) return
        
        var connection = {
            from: fromNode,
            to: toNode,
            progress: 0,
            timestamp: Date.now()
        }
        
        activeConnections.push(connection)
        activeConnectionsChanged()
    }
    
    function animateConnections() {
        var updatedConnections = []
        
        for (var i = 0; i < activeConnections.length; i++) {
            var conn = activeConnections[i]
            conn.progress += 0.05
            
            if (conn.progress < 1.0) {
                updatedConnections.push(conn)
            }
        }
        
        activeConnections = updatedConnections
        activeConnectionsChanged()
        topologyCanvas.requestPaint()
    }
    
    // Header
    Rectangle {
        id: header
        anchors.top: parent.top
        width: parent.width
        height: 35
        color: "#1a1a1a"
        radius: 8
        
        Text {
            anchors.centerIn: parent
            text: "🌐 Live Network Topology"
            color: "#66d9ff"
            font.pixelSize: 14
            font.weight: Font.Bold
        }
        
        // Node count indicator
        Rectangle {
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            width: 60
            height: 20
            radius: 10
            color: "#2a2a2a"
            border.color: "#404040"
            border.width: 1
            
            Text {
                anchors.centerIn: parent
                text: networkNodes.length + " nodes"
                color: "#ffffff"
                font.pixelSize: 10
            }
        }
    }
    
    // Main canvas for network visualization
    Canvas {
        id: topologyCanvas
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 5
        
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            
            // Draw grid background
            drawGrid(ctx)
            
            // Draw connections
            drawConnections(ctx)
            
            // Draw animated connections
            drawActiveConnections(ctx)
            
            // Draw nodes
            drawNodes(ctx)
        }
        
        function drawGrid(ctx) {
            ctx.strokeStyle = "#1a1a1a"
            ctx.lineWidth = 1
            
            // Vertical lines
            for (var x = 0; x < width; x += 40) {
                ctx.beginPath()
                ctx.moveTo(x, 0)
                ctx.lineTo(x, height)
                ctx.stroke()
            }
            
            // Horizontal lines
            for (var y = 0; y < height; y += 40) {
                ctx.beginPath()
                ctx.moveTo(0, y)
                ctx.lineTo(width, y)
                ctx.stroke()
            }
        }
        
        function drawConnections(ctx) {
            ctx.strokeStyle = "#404040"
            ctx.lineWidth = 1
            
            // Draw static connections between nodes
            for (var i = 0; i < networkNodes.length; i++) {
                var node = networkNodes[i]
                if (node.id !== "local") {
                    var localNode = getNodeById("local")
                    if (localNode) {
                        ctx.beginPath()
                        ctx.moveTo(localNode.x, localNode.y)
                        ctx.lineTo(node.x, node.y)
                        ctx.stroke()
                    }
                }
            }
        }
        
        function drawActiveConnections(ctx) {
            for (var i = 0; i < activeConnections.length; i++) {
                var conn = activeConnections[i]
                
                // Calculate animated position
                var x = conn.from.x + (conn.to.x - conn.from.x) * conn.progress
                var y = conn.from.y + (conn.to.y - conn.from.y) * conn.progress
                
                // Draw animated packet
                ctx.fillStyle = "#00ff88"
                ctx.beginPath()
                ctx.arc(x, y, 3, 0, 2 * Math.PI)
                ctx.fill()
                
                // Draw trail
                ctx.strokeStyle = "#00ff8840"
                ctx.lineWidth = 2
                ctx.beginPath()
                ctx.moveTo(conn.from.x, conn.from.y)
                ctx.lineTo(x, y)
                ctx.stroke()
            }
        }
        
        function drawNodes(ctx) {
            for (var i = 0; i < networkNodes.length; i++) {
                var node = networkNodes[i]
                
                // Node circle
                var radius = node.type === "local" ? 15 : 12
                
                // Outer glow
                ctx.fillStyle = node.color + "40"
                ctx.beginPath()
                ctx.arc(node.x, node.y, radius + 5, 0, 2 * Math.PI)
                ctx.fill()
                
                // Main node
                ctx.fillStyle = node.color
                ctx.beginPath()
                ctx.arc(node.x, node.y, radius, 0, 2 * Math.PI)
                ctx.fill()
                
                // Inner highlight
                ctx.fillStyle = "#ffffff60"
                ctx.beginPath()
                ctx.arc(node.x - 3, node.y - 3, radius * 0.4, 0, 2 * Math.PI)
                ctx.fill()
                
                // Node type icon
                ctx.fillStyle = "#000000"
                ctx.font = "12px Arial"
                ctx.textAlign = "center"
                var icon = getNodeIcon(node.type)
                ctx.fillText(icon, node.x, node.y + 4)
                
                // IP address label
                ctx.fillStyle = "#ffffff"
                ctx.font = "9px Consolas"
                ctx.textAlign = "center"
                ctx.fillText(node.ip, node.x, node.y + radius + 15)
                
                // Connection count
                if (node.connections > 0) {
                    ctx.fillStyle = "#ff6b6b"
                    ctx.beginPath()
                    ctx.arc(node.x + radius - 3, node.y - radius + 3, 6, 0, 2 * Math.PI)
                    ctx.fill()
                    
                    ctx.fillStyle = "#ffffff"
                    ctx.font = "8px Arial"
                    ctx.textAlign = "center"
                    ctx.fillText(node.connections.toString(), node.x + radius - 3, node.y - radius + 6)
                }
            }
        }
        
        function getNodeIcon(type) {
            switch (type) {
                case "local": return "💻"
                case "router": return "📡"
                case "server": return "🖥️"
                case "device": return "📱"
                default: return "❓"
            }
        }
        
        // Mouse interaction
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            
            property var hoveredNode: null
            
            onPositionChanged: (mouse) => {
                // Find hovered node
                hoveredNode = null
                for (var i = 0; i < networkNodes.length; i++) {
                    var node = networkNodes[i]
                    var dx = mouse.x - node.x
                    var dy = mouse.y - node.y
                    var distance = Math.sqrt(dx * dx + dy * dy)
                    
                    if (distance < 15) {
                        hoveredNode = node
                        break
                    }
                }
                
                // Update cursor
                cursorShape = hoveredNode ? Qt.PointingHandCursor : Qt.ArrowCursor
            }
            
            onClicked: (mouse) => {
                if (hoveredNode) {
                    console.log("🌐 Clicked node:", hoveredNode.ip, hoveredNode.type)
                    // Could open node details dialog here
                }
            }
        }
    }
    
    // Legend
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        width: 120
        height: 80
        color: "#1a1a1a80"
        radius: 6
        border.color: "#404040"
        border.width: 1
        
        Column {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 4
            
            Text {
                text: "Legend"
                color: "#ffffff"
                font.pixelSize: 10
                font.weight: Font.Bold
            }
            
            Row {
                spacing: 6
                Rectangle { width: 8; height: 8; radius: 4; color: "#00ff88" }
                Text { text: "Local"; color: "#ffffff"; font.pixelSize: 8 }
            }
            
            Row {
                spacing: 6
                Rectangle { width: 8; height: 8; radius: 4; color: "#66d9ff" }
                Text { text: "Router"; color: "#ffffff"; font.pixelSize: 8 }
            }
            
            Row {
                spacing: 6
                Rectangle { width: 8; height: 8; radius: 4; color: "#ffc107" }
                Text { text: "Server"; color: "#ffffff"; font.pixelSize: 8 }
            }
            
            Row {
                spacing: 6
                Rectangle { width: 8; height: 8; radius: 4; color: "#9E9E9E" }
                Text { text: "Device"; color: "#ffffff"; font.pixelSize: 8 }
            }
        }
    }
    
    // Public functions
    function startCapture() {
        isCapturing = true
        console.log("🌐 Network topology monitoring started")
    }
    
    function stopCapture() {
        isCapturing = false
        activeConnections = []
        console.log("🌐 Network topology monitoring stopped")
    }
    
    function addPacketFlow(sourceIP, destIP) {
        var sourceNode = null
        var destNode = null
        
        // Find nodes by IP
        for (var i = 0; i < networkNodes.length; i++) {
            if (networkNodes[i].ip === sourceIP) sourceNode = networkNodes[i]
            if (networkNodes[i].ip === destIP) destNode = networkNodes[i]
        }
        
        // Create nodes if they don't exist
        if (!sourceNode && sourceIP !== "0.0.0.0") {
            sourceNode = {
                id: "auto_" + sourceIP.replace(/\./g, "_"),
                ip: sourceIP,
                type: "device",
                x: width * 0.3, // Fixed position - no random placement
                y: height * 0.6,
                connections: 0,
                lastSeen: Date.now(),
                color: "#9E9E9E"
            }
            networkNodes.push(sourceNode)
        }
        
        if (!destNode && destIP !== "0.0.0.0") {
            destNode = {
                id: "auto_" + destIP.replace(/\./g, "_"),
                ip: destIP,
                type: "server",
                x: width * 0.7, // Fixed position - no random placement
                y: height * 0.4,
                connections: 0,
                lastSeen: Date.now(),
                color: "#ff66b3"
            }
            networkNodes.push(destNode)
        }
        
        // Animate connection
        if (sourceNode && destNode) {
            addConnectionAnimation(sourceNode, destNode)
            sourceNode.connections++
            destNode.connections++
        }
        
        networkNodesChanged()
    }
}
           