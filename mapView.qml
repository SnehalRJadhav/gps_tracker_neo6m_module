import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    width: parent.width
    height: parent.height

    Plugin{
        id : mapPlugin
        name: 'osm'
    }

    Map{
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(19.0761, 72.8775)
        zoomLevel: 14

        Connections {
            target: serialHandler
            function onCurrentCoordinateChanged(coords) {
                console.log("QML Received:", coords.latitude, coords.longitude);
                map.center = coords
                marker.coordinate = coords

            }
            function onCurrentDateTimeChanged(dateTime){
                console.log("QML Date time : ", serialHandler.currentDateTime)
            }
        }

        MapQuickItem{
            id: marker
            anchorPoint.x: markerRect.width/2
            anchorPoint.y: markerRect.height
            coordinate: {
                if (serialHandler.currentCoordinate) {
                    return serialHandler.currentCoordinate.isValid ?
                                serialHandler.currentCoordinate :
                                QtPositioning.coordinate(0.0, 0.0);
                } else {
                    return QtPositioning.coordinate(0.0, 0.0);
                }
            }
            sourceItem: Rectangle{
                id:markerRect
                width:20
                height:20
                radius: 20
                color: "red"
            }
        }
        PinchHandler {
            target: null
            grabPermissions: PointerHandler.TakeOverForbidden
            property geoCoordinate startCenteroid
            onActiveChanged: {
                if (active)
                    startCenteroid = map.toCoordinate(centroid.position, false)
            }
            onScaleChanged: (delta) => {
                map.zoomLevel += Math.log(delta)
                map.alignCoordinateToPoint(startCenteroid, centroid.position)
            }
        }
        WheelHandler {
            onWheel: function(event) {
                const loc = map.toCoordinate(point.position)
                map.zoomLevel += event.angleDelta.y / 120;
                map.alignCoordinateToPoint(loc, point.position)
            }
        }
        DragHandler {
            target: null
            grabPermissions: PointerHandler.TakeOverForbidden
            onTranslationChanged: (delta) => { map.pan(-delta.x, -delta.y); }
        }
    }
    Rectangle{
        id: coordRect
        width: 200
        height: 60
        color: "white"
        anchors.top: parent.top
        anchors.left:parent.left

        Text {
            id: coordTxt
            anchors.centerIn: parent
            text: {
                if(!serialHandler.currentCoordinate){
                    console.log("serialHandler.getCurrentCoordinate : ",serialHandler.currentCoordinate)
                    return "Waiting for GPS ..."
                }
                if(!serialHandler.currentCoordinate.isValid){
                    return "Invalid Coordinates ..."
                }
                return `Lat: ${serialHandler.currentCoordinate.latitude.toFixed(6)}\nLon: ${serialHandler.currentCoordinate.longitude.toFixed(6)}`

            }
        }
    }

    Rectangle{
        id: dateTimeRect
        width : 300
        height: 60
        color: "white"
        anchors.top: parent.top
        anchors.right:parent.right

        Text {
            id: dateTime
            anchors.centerIn: parent
            text:serialHandler.currentDateTime
        }
        MouseArea{
            anchors.fill:parent
            onClicked:{
                console.log("Date time rect")
            }
        }
    }

}
