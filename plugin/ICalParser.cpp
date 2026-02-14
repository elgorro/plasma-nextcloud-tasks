// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#include "ICalParser.h"

#include <QRegularExpression>
#include <QXmlStreamReader>

namespace ICalParser {

QList<CalendarInfo> parseCalendars(const QByteArray &xml, const QString &origin)
{
    QList<CalendarInfo> calendars;
    QXmlStreamReader reader(xml);

    while (!reader.atEnd()) {
        reader.readNext();

        if (!reader.isStartElement() || reader.name() != u"response") {
            continue;
        }

        // Parse one <response> block
        QString href;
        QString displayName;
        bool isCalendar = false;
        bool vtodoSupported = false;
        bool foundComponentSet = false;

        while (!reader.atEnd()) {
            reader.readNext();

            if (reader.isEndElement() && reader.name() == u"response") {
                break;
            }

            if (!reader.isStartElement()) {
                continue;
            }

            if (reader.name() == u"href") {
                href = reader.readElementText();
            } else if (reader.name() == u"displayname") {
                displayName = reader.readElementText();
            } else if (reader.name() == u"calendar") {
                isCalendar = true;
            } else if (reader.name() == u"supported-calendar-component-set") {
                foundComponentSet = true;
            } else if (reader.name() == u"comp") {
                auto name = reader.attributes().value(u"name");
                if (name.compare(u"VTODO", Qt::CaseInsensitive) == 0) {
                    vtodoSupported = true;
                }
            }
        }

        if (!isCalendar) {
            continue;
        }

        // If no component-set element was found, assume VTODO support
        if (!foundComponentSet) {
            vtodoSupported = true;
        }

        if (!vtodoSupported || href.isEmpty()) {
            continue;
        }

        // Resolve href: if it's an absolute path, prepend the origin
        QString fullUrl = href;
        if (href.startsWith(u'/') && !href.contains(u"://")) {
            fullUrl = origin + href;
        }

        if (displayName.isEmpty()) {
            // Extract last path component as fallback name
            auto parts = href.split(u'/', Qt::SkipEmptyParts);
            displayName = parts.isEmpty() ? QStringLiteral("Calendar") : parts.last();
        }

        calendars.append({fullUrl, displayName});
    }

    return calendars;
}

static Task parseVTodoBlock(const QString &vtodoContent, const QString &calendarName)
{
    auto lines = vtodoContent.split(QRegularExpression(QStringLiteral("\\r?\\n")));
    QHash<QString, QString> props;

    for (const auto &rawLine : lines) {
        auto line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        int colonIdx = line.indexOf(u':');
        if (colonIdx == -1) {
            continue;
        }

        QString key = line.left(colonIdx);
        QString value = line.mid(colonIdx + 1);

        // Strip parameters (e.g., "DTSTART;VALUE=DATE" → "DTSTART")
        int semiIdx = key.indexOf(u';');
        if (semiIdx != -1) {
            key = key.left(semiIdx);
        }

        // Only store the first occurrence
        if (!props.contains(key)) {
            props.insert(key, value);
        }
    }

    Task task;
    task.uid = props.value(QStringLiteral("UID"));
    if (task.uid.isEmpty()) {
        return {};
    }

    task.title = props.value(QStringLiteral("SUMMARY"));
    task.description = props.value(QStringLiteral("DESCRIPTION"));

    auto due = props.value(QStringLiteral("DUE"));
    task.dueDate = due.isEmpty() ? QString() : parseICalDate(due);

    bool ok = false;
    task.priority = props.value(QStringLiteral("PRIORITY"), QStringLiteral("0")).toInt(&ok);
    if (!ok) {
        task.priority = 0;
    }

    task.completed = props.value(QStringLiteral("STATUS")).compare(
                         QStringLiteral("COMPLETED"), Qt::CaseInsensitive) == 0;
    task.calendarName = calendarName;

    return task;
}

QList<Task> parseTasks(const QByteArray &xml, const QString &calendarName)
{
    QList<Task> tasks;
    QXmlStreamReader reader(xml);

    while (!reader.atEnd()) {
        reader.readNext();

        if (!reader.isStartElement() || reader.name() != u"response") {
            continue;
        }

        // Find calendar-data within this response
        while (!reader.atEnd()) {
            reader.readNext();

            if (reader.isEndElement() && reader.name() == u"response") {
                break;
            }

            if (reader.isStartElement() && reader.name() == u"calendar-data") {
                QString icalData = reader.readElementText();

                // Unfold and extract VTODO blocks
                QString unfolded = unfoldICalLines(icalData);
                static const QRegularExpression vtodoRe(
                    QStringLiteral("BEGIN:VTODO([\\s\\S]*?)END:VTODO"));
                auto it = vtodoRe.globalMatch(unfolded);

                while (it.hasNext()) {
                    auto match = it.next();
                    Task task = parseVTodoBlock(match.captured(1), calendarName);
                    if (!task.uid.isEmpty()) {
                        tasks.append(task);
                    }
                }
            }
        }
    }

    return tasks;
}

QString unfoldICalLines(const QString &text)
{
    // RFC 5545: lines starting with a space or tab are continuations
    static const QRegularExpression foldRe(QStringLiteral("\\r?\\n[ \\t]"));
    QString result = text;
    result.replace(foldRe, QString());
    return result;
}

QString parseICalDate(const QString &icalDate)
{
    auto trimmed = icalDate.trimmed();

    // Full datetime: 20240115T103000Z or 20240115T103000
    static const QRegularExpression dtRe(
        QStringLiteral("^(\\d{4})(\\d{2})(\\d{2})T(\\d{2})(\\d{2})(\\d{2})(Z?)$"));
    auto m = dtRe.match(trimmed);
    if (m.hasMatch()) {
        return m.captured(1) + u'-' + m.captured(2) + u'-' + m.captured(3)
               + u'T' + m.captured(4) + u':' + m.captured(5) + u':' + m.captured(6)
               + m.captured(7);
    }

    // Date only: 20240115
    static const QRegularExpression dateRe(QStringLiteral("^(\\d{4})(\\d{2})(\\d{2})$"));
    m = dateRe.match(trimmed);
    if (m.hasMatch()) {
        return m.captured(1) + u'-' + m.captured(2) + u'-' + m.captured(3);
    }

    return trimmed;
}

} // namespace ICalParser
