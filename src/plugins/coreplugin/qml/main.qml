import QtQuick 2.15
import qsdf.gui.window 1.0

FramelessQuickWindow {
    width: 1280
    height: 720
    visible: true
    title: qsTr("Hello World")

    Loader {
        id: domainLoader
        anchors.fill: parent
    }
}
