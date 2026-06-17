import QtQuick
import QtQuick.Controls

Window {
    width: 400
    height: 300
    visible: true
    title: "Button Counter Application"

    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            id: counterText
            // Binds the text directly to the C++ value
            text: "Count: " + backendCounter.value 
            font.pixelSize: 32
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Row {
            spacing: 20
            Button {
                text: "Increment"
                width: 100
                height: 50
                // Calls the C++ slot when clicked
                onClicked: backendCounter.increment() 
            }
            Button {
                text: "Reset"
                width: 100
                height: 50
                // Calls the C++ slot when clicked
                onClicked: backendCounter.reset() 
            }
        }
    }
}