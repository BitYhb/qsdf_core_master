import QtQuick
import ${TM_TARGET_NAME}

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("${TM_TARGET_NAME}")

    Viewer {
        anchors.fill: parent
    }
}
