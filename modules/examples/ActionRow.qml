import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    width: 400
    height: 30

    Row {
        id: row
        anchors.fill: parent



        Text {
            id: actionName
            width: 100
            height: 30
            text: qsTr("Action")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 11
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        Text {
            id: actionStatus
            width: 100
            height: 30
            text: qsTr("Status")
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 11
            horizontalAlignment: Text.AlignHCenter
            anchors.left: actionName.right
            anchors.leftMargin: 0
        }
        Rectangle {
            id: actionStatusColor
            width: 100
            height: 30
            color: "#30e6fd"
            anchors.left: actionStatus.right
            anchors.leftMargin: 0
        }

        Button {
            id: toggleButton
            text: qsTr("Toggle")
            font.pointSize: 11
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: actionStatusColor.right
            anchors.leftMargin: 0
        }
    }

}
