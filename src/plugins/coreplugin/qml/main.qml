import QtQuick

// qsdf-core
import qsdf.gui.window

FramelessQuickWindow {
    id: rootWindow
    width: 1280
    height: 720
    visible: false
    title: qsTr("Hello World")

    Loader {
        id: domainLoader
        anchors.fill: parent
        active: rootWindow.domainViewURI.length > 0
        source: rootWindow.domainViewURI
        onLoaded: {
            rootWindow.visible = true
        }
    }
}
