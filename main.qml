import QtQuick 2.15
import QtLocation 5.15
import QtPositioning 5.15
import QtQuick.Controls 2.15

Window {
    id:mainWindow
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    visible:true

    property var coordinateList:[]

    Plugin {
        id: mapPlugin
        name: "osm"

        PluginParameter {
            name: "osm.mapping.providersrepository.disabled"
            value: true
        }

        PluginParameter {
            name: "osm.mapping.tileserver.url"
            value: "https://tiles.stadiamaps.com/tiles/alidade_smooth_dark/{z}/{x}/{y}.png"
        }
    }

    Map{
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(19.0761, 72.8775)
        zoomLevel: 16

        MapPolyline {
            id: pathLine
            line.width: 3
            line.color: "red"
            path: mainWindow.coordinateList
        }
        Connections {
            target: serialHandler
            function onCurrentCoordinateChanged(coords) {
                map.center = coords
                marker.coordinate = coords
                mainWindow.coordinateList.push(coords)

            }
            function onCurrentDateTimeChanged(dateTime){
                console.log("QML Date time : ", serialHandler.currentDateTime)
            }
            function onCompassHeadingChanged(heading){
                console.log("Compass heading : ",heading)
            }
            function onSatelliteCountChanged(count){
                console.log("serialHandler sat count : ",serialHandler.satelliteCount)
            }
        }

        MapQuickItem{
            id: marker
            anchorPoint.x: markerImage.width/2
            anchorPoint.y: markerImage.height
            coordinate: {
                if (serialHandler.currentCoordinate) {
                    return serialHandler.currentCoordinate.isValid ?
                                serialHandler.currentCoordinate :
                                QtPositioning.coordinate(0.0, 0.0);
                } else {
                    return QtPositioning.coordinate(0.0, 0.0);
                }
            }
            sourceItem: Image {
                id: markerImage
                source: "assets/location_pin.png"
                width:32
                height: 32
                fillMode: Image.PreserveAspectFit
                smooth: true
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
        id:detailRect
        width:parent.width * 0.2
        height:parent.height
        color : "transparent"
        anchors{
            right:parent.right
        }
        Column {
            width: parent.width
            spacing: detailRect.height * 0.04
            anchors {
                top: parent.top
                topMargin: detailRect.height * 0.04
                horizontalCenter: parent.horizontalCenter
            }

            Rectangle {
                id: dateTimeRect
                width: detailRect.width * 0.9
                height: detailRect.height * 0.1
                color: "#8b8b8b66"
                radius: 20
                anchors.horizontalCenter: parent.horizontalCenter
                Text {
                    id: dateTime
                    anchors.centerIn: parent
                    text:serialHandler.currentDateTime
                    font.pixelSize:14
                }
                // MouseArea{
                //     anchors.fill: parent
                //     onClicked:{
                //         console.log("In mouse area")
                //         coordinateList.push(QtPositioning.coordinate(19.077064, 72.998993))
                //         coordinateList.push(QtPositioning.coordinate(19.0761, 72.8775))
                //          coordinateList = coordinateList
                //     }
                // }
            }

            Rectangle {
                id: gpsDataRect
                width: detailRect.width * 0.9
                height: detailRect.height * 0.25
                color: "#8b8b8b66"
                radius: 20
                anchors.horizontalCenter: parent.horizontalCenter
                Column{
                    width: parent.width
                    spacing: parent.height * 0.1
                    anchors {
                        left:parent.left
                        leftMargin: parent.width * 0.1
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    Text {
                        id: latTxt
                        text:"<b>Lat : </b>"+serialHandler.currentCoordinate.latitude.toFixed(6)
                        font.pixelSize:14
                    }
                    Text {
                        id: lonTxt
                        text:"<b>Lon : </b>"+serialHandler.currentCoordinate.longitude.toFixed(6)
                        font.pixelSize:14
                    }
                    Text {
                        id: satelliteTxt
                        text:"<b>Satellites : </b>"+serialHandler.satelliteCount
                        font.pixelSize:14
                    }
                    Text {
                        id: speedTxt
                        text:"<b>Speed : </b>"+serialHandler.speed.toFixed(6) +" kmph"
                        font.pixelSize:14
                    }
                }
            }
            Rectangle {
                id: compassRect
                width: detailRect.width * 0.9
                height: detailRect.height * 0.3
                color: "transparent"//"#8b8b8b66"
                radius: 20
                anchors.horizontalCenter: parent.horizontalCenter
                CompassView{
                    width:parent.width
                    height:parent.height
                    anchors.centerIn: parent
                    needleRotation : serialHandler.compassHeading
                }
            }
        }
    }
}
