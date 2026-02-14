import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents
import org.kde.plasma.extras as PlasmaExtras

PlasmaExtras.PlasmoidHeading {
    id: root

    required property var taskModel

    contentItem: RowLayout {
        PlasmaExtras.Heading {
            Layout.fillWidth: true
            level: 1
            text: i18n("Nextcloud Tasks")
        }

        PlasmaComponents.Label {
            visible: root.taskModel.lastSyncTime !== ""
            text: root.taskModel.lastSyncTime
            opacity: 0.6
            font: Kirigami.Theme.smallFont
        }

        Kirigami.Icon {
            implicitWidth: Kirigami.Units.iconSizes.smallMedium
            implicitHeight: Kirigami.Units.iconSizes.smallMedium
            source: "data-warning"
            color: Kirigami.Theme.negativeTextColor
            visible: root.taskModel.lastError !== ""

            PlasmaComponents.ToolTip {
                text: root.taskModel.lastError
            }
        }

        PlasmaComponents.ToolButton {
            icon.name: "view-refresh"
            onClicked: root.taskModel.sync()
            enabled: !root.taskModel.syncing

            PlasmaComponents.ToolTip {
                text: i18n("Refresh Tasks")
            }
        }
    }
}
