// PacketTableView.qml - Packet list table view
import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: listView
    
    property alias model: listView.model
    
    clip: true
    
    // Header
    header: Rectangle {
        width: listView.width
        height: 40
        color: "#333333"
        
        Row {
            anchors.fill: parent
            anchors.margins: 5
            
            Text { text: "Time"; color: "white"; font.bold: true; width: 120 }
            Text { text: "Source"; color: "white"; font.bold: true; width: 120 }
            Text { text: "Destination"; color: "white"; font.bold: true; width: 120 }
            Text { text: "Protocol"; color: "white"; font.bold: true; width: 80 }
            Text { text: "Size"; color: "white"; font.bold: true; width: 80 }
            Text { text: "Info"; color: "white"; font.bold: true; width: 200 }
        }
    }
    
    delegate: Rectangle {
        width: listView.width
        height: 30
        color: index % 2 === 0 ? "#2a2a2a" : "#333333"
        
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            
            onEntered: parent.color = "#444444"
            onExited: parent.color = index % 2 === 0 ? "#2a2a2a" : "#333333"
            onClicked: listView.currentIndex = index
        }
        
        Row {
            anchors.fill: parent
            anchors.margins: 5
            
            Text { 
                text: model.timestamp ? model.timestamp.substring(model.timestamp.length - 8) : ""
                color: "white"
                font.pixelSize: 11
                width: 120
                elide: Text.ElideRight
            }
            Text { 
                text: model.sourceIP || ""
                color: "#88ccff"
                font.pixelSize: 11
                width: 120
                elide: Text.ElideRight
            }
            Text { 
                text: model.destIP || ""
                color: "#ffcc88"
                font.pixelSize: 11
                width: 120
                elide: Text.ElideRight
            }
            Text { 
                text: model.protocol || ""
                color: "#88ff88"
                font.pixelSize: 11
                width: 80
                elide: Text.ElideRight
            }
            Text { 
                text: model.size ? model.size.toString() : ""
                color: "white"
                font.pixelSize: 11
                width: 80
                elide: Text.ElideRight
            }
            Text { 
                text: model.info || ""
                color: "#cccccc"
                font.pixelSize: 11
                width: 200
                elide: Text.ElideRight
            }
        }
    }
    
    ScrollBar.vertical: ScrollBar {
        active: true
        policy: ScrollBar.AlwaysOn
    }
}