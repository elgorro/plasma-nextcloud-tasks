import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    property alias cfg_serverUrl: serverUrlField.text
    property alias cfg_username: usernameField.text
    property alias cfg_appPassword: appPasswordField.text
    property alias cfg_syncIntervalMinutes: syncIntervalSpinBox.value
    property alias cfg_autoSync: autoSyncCheckBox.checked

    Kirigami.FormLayout {
        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Server")
        }

        QQC2.TextField {
            id: serverUrlField
            Kirigami.FormData.label: i18n("Server URL:")
            placeholderText: "https://cloud.example.com"
        }

        QQC2.TextField {
            id: usernameField
            Kirigami.FormData.label: i18n("Username:")
        }

        QQC2.TextField {
            id: appPasswordField
            Kirigami.FormData.label: i18n("App password:")
            echoMode: TextInput.Password
        }

        Kirigami.Separator {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Sync")
        }

        QQC2.CheckBox {
            id: autoSyncCheckBox
            Kirigami.FormData.label: i18n("Automatic sync:")
        }

        QQC2.SpinBox {
            id: syncIntervalSpinBox
            Kirigami.FormData.label: i18n("Sync interval (minutes):")
            from: 1
            to: 1440
            enabled: autoSyncCheckBox.checked
        }
    }
}
