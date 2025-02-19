import QtQuick 2.15

Item {
    id: rootItem
    width : parent.width
    height : parent.height
    Text {
        id: headerText
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.leftMargin : parent.width * 0.5
        text: qsTr("GPS Data")
        font.pixelSize: 16
        font.bold : true
    }

    ListView{
        id:listView
        width:parent.width * 0.8
        height:parent.height * 0.8
        anchors.left:parent.left
        anchors.leftMargin:parent.width * 0.1
        anchors.top:headerText.bottom
        anchors.topMargin:parent.height * 0.1

        Component{
            id: listDelegate
            Item {
                id: myItem
                width: listView.width * 0.8
                height: listView.height * 0.05
                Text{
                    id:dText
                    text:model.modelData
                }
                Rectangle{
                    id: seperater
                    width:parent.width
                    height:1
                    anchors.top:dText.bottom
                    anchors.topMargin: myItem.height * 0.3
                    border.color:"black"
                }
            }
        }

        model:serialHandler.sentence
        delegate: listDelegate

    }

}
