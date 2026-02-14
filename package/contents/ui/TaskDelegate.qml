import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.extras as PlasmaExtras

PlasmaExtras.ExpandableListItem {
    id: root

    opacity: model.completed ? 0.5 : 1.0

    icon: _priorityIcon(model.priority)
    title: model.title
    subtitle: _formatSubtitle(model.dueDate, model.calendarName)
    customExpandedViewContent: Component {
        Loader {
            active: model.description !== ""
            sourceComponent: PlasmaExtras.Heading {
                level: 5
                text: model.description
                wrapMode: Text.WordWrap
                opacity: 0.8
            }
        }
    }
    isDefault: !model.completed

    function _priorityIcon(priority) {
        if (priority >= 1 && priority <= 3) {
            return "flag-red";
        }
        if (priority >= 4 && priority <= 6) {
            return "flag-yellow";
        }
        if (priority >= 7 && priority <= 9) {
            return "flag-green";
        }
        return "view-task";
    }

    function _formatSubtitle(dueDate, calendarName) {
        var parts = [];
        if (calendarName !== "") {
            parts.push(calendarName);
        }
        if (dueDate !== "") {
            var d = new Date(dueDate);
            parts.push(d.toLocaleDateString(Qt.locale(), Locale.ShortFormat));
        }
        return parts.join(" \u2022 ");
    }
}
