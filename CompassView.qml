import QtQuick 2.15

Rectangle {
    width:parent.width
    height:parent.height
    color:"transparent"

    property real needleRotation: 0
    Image {
        id: compassBackground
        source: "assets/compass_view.png"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
        fillMode: Image.PreserveAspectFit
    }
    Image {
        id: needle
        source: "assets/needle.png"
        width: parent.width * 0.9
        height: parent.height * 0.5
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        transform: Rotation {
            origin.x: needle.width / 2
            origin.y: needle.height / 2
            angle: needleRotation
        }
    }
}
