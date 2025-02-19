import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    id: window
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    visible: true
    visibility: Window.Maximized
    title: qsTr("GPS Tracker")


    TabBar {
        id: tabBar
        width: parent.width
        height: 50
        anchors.top: parent.top

        TabButton {
            text: "Gps Data"
            onClicked: stackView.push("gpsData.qml")
        }

        TabButton {
            text: "Map View"
            onClicked: stackView.push(Qt.resolvedUrl("mapView.qml"))
        }
    }


    StackView {
        id: stackView
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        initialItem : "gpsData.qml"
    }
}
