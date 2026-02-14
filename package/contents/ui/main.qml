import QtQuick
import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import com.github.elgorro.nextcloudtasks

PlasmoidItem {
    id: root

    Plasmoid.configurationRequired: Plasmoid.configuration.serverUrl === ""

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18n("Refresh Tasks")
            icon.name: "view-refresh"
            onTriggered: taskModel.sync()
        }
    ]

    TaskModel {
        id: taskModel
        serverUrl: Plasmoid.configuration.serverUrl
        username: Plasmoid.configuration.username
        appPassword: Plasmoid.configuration.appPassword
        syncIntervalMinutes: Plasmoid.configuration.syncIntervalMinutes
        autoSync: Plasmoid.configuration.autoSync
    }

    compactRepresentation: CompactRepresentation {
        taskModel: taskModel
    }

    fullRepresentation: FullRepresentation {
        taskModel: taskModel
    }
}
