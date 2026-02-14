import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents

MouseArea {
    id: root

    required property var taskModel

    hoverEnabled: true
    onClicked: expanded = !expanded

    Kirigami.Icon {
        anchors.fill: parent
        source: "view-task"
        active: root.containsMouse
    }

    PlasmaComponents.BusyIndicator {
        anchors.fill: parent
        running: root.taskModel.syncing
        visible: running
    }
}
