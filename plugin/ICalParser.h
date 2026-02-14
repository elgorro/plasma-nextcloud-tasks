// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#ifndef ICALPARSER_H
#define ICALPARSER_H

#include <QByteArray>
#include <QList>
#include <QString>

struct CalendarInfo {
    QString url;
    QString name;
};

struct Task {
    QString uid;
    QString title;
    QString description;
    QString dueDate;
    int priority = 0;
    bool completed = false;
    QString calendarName;
};

namespace ICalParser {

QList<CalendarInfo> parseCalendars(const QByteArray &xml, const QString &origin);
QList<Task> parseTasks(const QByteArray &xml, const QString &calendarName);
QString unfoldICalLines(const QString &text);
QString parseICalDate(const QString &icalDate);

} // namespace ICalParser

#endif // ICALPARSER_H
