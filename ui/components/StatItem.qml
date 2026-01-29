// StatItem.qml - Statistics display item
import QtQuick 2.15

Row {
    property string label: "Label:"
    property var value: "Value"
    
    spacing: 5
    
    Text {
        text: label
        color: "#888888"
        font.pixelSize: 12
        width: 60
    }
    
    Text {
        text: typeof value === 'number' ? value.toLocaleString() : value.toString()
        color: "white"
        font.pixelSize: 12
        font.bold: true
    }
}