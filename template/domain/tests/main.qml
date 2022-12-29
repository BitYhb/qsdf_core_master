import QtQuick 2.15
import EPDomain_okr 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Viewer {
        anchors.fill: parent
    }
}
