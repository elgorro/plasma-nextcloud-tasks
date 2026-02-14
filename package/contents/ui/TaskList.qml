import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

PlasmaComponents.ScrollView {
    id: root

    required property var taskModel

    ListView {
        id: listView
        model: root.taskModel
        clip: true

        delegate: TaskDelegate {}

        PlasmaExtras.PlaceholderMessage {
            anchors.centerIn: parent
            width: parent.width - Kirigami.Units.gridUnit * 4
            visible: !root.taskModel.syncing && listView.count === 0

            iconName: root.taskModel.lastError !== ""
                      ? "network-disconnect"
                      : "checkmark"
            text: {
                if (root.taskModel.lastError !== "") {
                    return i18n("Sync failed");
                }
                return i18n("No tasks");
            }
        }

        PlasmaComponents.BusyIndicator {
            anchors.centerIn: parent
            running: root.taskModel.syncing && listView.count === 0
            visible: running
        }
    }
}
