import QtQuick
import QtQuick.Layouts
import org.kde.plasma.plasmoid
import org.kde.plasma.components as PlasmaComponents

PlasmoidItem {
    Plasmoid.fullRepresentation: Item {
        Layout.minimumWidth: 300
        Layout.minimumHeight: 200
        Layout.preferredWidth: 400
        Layout.preferredHeight: 300

        PlasmaComponents.Label {
            anchors.centerIn: parent
            text: "Nextcloud Tasks"
        }
    }
}
