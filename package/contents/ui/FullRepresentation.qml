import QtQuick
import QtQuick.Layouts
import org.kde.plasma.extras as PlasmaExtras

PlasmaExtras.Representation {
    id: root

    required property var taskModel

    Layout.minimumWidth: 300
    Layout.minimumHeight: 200
    Layout.preferredWidth: 400
    Layout.preferredHeight: 400

    header: Header {
        taskModel: root.taskModel
    }

    TaskList {
        anchors.fill: parent
        taskModel: root.taskModel
    }
}
