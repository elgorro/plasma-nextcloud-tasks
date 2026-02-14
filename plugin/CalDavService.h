// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#ifndef CALDAVSERVICE_H
#define CALDAVSERVICE_H

#include "ICalParser.h"

#include <QNetworkAccessManager>
#include <QObject>

class CalDavService : public QObject
{
    Q_OBJECT

public:
    explicit CalDavService(QObject *parent = nullptr);

    void fetchTasks(const QString &serverUrl, const QString &username, const QString &password);

Q_SIGNALS:
    void tasksFetched(const QList<Task> &tasks);
    void fetchError(const QString &message);

private:
    void discoverCalendars(const QString &davUrl, const QString &origin,
                           const QString &username, const QString &password);
    void fetchCalendarTasks(const QList<CalendarInfo> &calendars,
                            const QString &username, const QString &password);

    QNetworkAccessManager m_nam;
    QList<Task> m_pendingTasks;
    int m_pendingCalendars = 0;
};

#endif // CALDAVSERVICE_H
