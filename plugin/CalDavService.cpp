// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#include "CalDavService.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>

CalDavService::CalDavService(QObject *parent)
    : QObject(parent)
{
}

void CalDavService::fetchTasks(const QString &serverUrl, const QString &username,
                                const QString &password)
{
    // Normalize URL: strip trailing slashes
    QString baseUrl = serverUrl;
    while (baseUrl.endsWith(u'/')) {
        baseUrl.chop(1);
    }

    QString davUrl = baseUrl + QStringLiteral("/remote.php/dav/calendars/")
                     + QString::fromUtf8(QUrl::toPercentEncoding(username)) + u'/';

    // Extract origin (scheme + host + port)
    QUrl parsed(baseUrl);
    QString origin = parsed.scheme() + QStringLiteral("://") + parsed.authority();

    discoverCalendars(davUrl, origin, username, password);
}

void CalDavService::discoverCalendars(const QString &davUrl, const QString &origin,
                                       const QString &username, const QString &password)
{
    QByteArray body =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\" "
        "            xmlns:c=\"urn:ietf:params:xml:ns:caldav\">"
        "  <d:prop>"
        "    <d:resourcetype />"
        "    <d:displayname />"
        "    <c:supported-calendar-component-set />"
        "  </d:prop>"
        "</d:propfind>";

    QNetworkRequest request{QUrl(davUrl)};
    request.setRawHeader("Authorization",
                         "Basic " + (username + u':' + password).toUtf8().toBase64());
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QStringLiteral("application/xml; charset=utf-8"));
    request.setRawHeader("Depth", "1");

    auto *reply = m_nam.sendCustomRequest(request, "PROPFIND", body);

    connect(reply, &QNetworkReply::finished, this,
            [this, reply, origin, username, password]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            Q_EMIT fetchError(QStringLiteral("Server error: ") + reply->errorString());
            return;
        }

        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status != 207) {
            Q_EMIT fetchError(QStringLiteral("Server error: %1").arg(status));
            return;
        }

        QByteArray data = reply->readAll();
        auto calendars = ICalParser::parseCalendars(data, origin);

        if (calendars.isEmpty()) {
            Q_EMIT tasksFetched({});
            return;
        }

        fetchCalendarTasks(calendars, username, password);
    });
}

void CalDavService::fetchCalendarTasks(const QList<CalendarInfo> &calendars,
                                        const QString &username, const QString &password)
{
    m_pendingTasks.clear();
    m_pendingCalendars = calendars.size();

    QByteArray body =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">"
        "  <d:prop>"
        "    <d:getetag />"
        "    <c:calendar-data />"
        "  </d:prop>"
        "  <c:filter>"
        "    <c:comp-filter name=\"VCALENDAR\">"
        "      <c:comp-filter name=\"VTODO\" />"
        "    </c:comp-filter>"
        "  </c:filter>"
        "</c:calendar-query>";

    QByteArray authHeader = "Basic " + (username + u':' + password).toUtf8().toBase64();

    for (const auto &cal : calendars) {
        QNetworkRequest request{QUrl(cal.url)};
        request.setRawHeader("Authorization", authHeader);
        request.setHeader(QNetworkRequest::ContentTypeHeader,
                          QStringLiteral("application/xml; charset=utf-8"));
        request.setRawHeader("Depth", "1");

        auto *reply = m_nam.sendCustomRequest(request, "REPORT", body);
        QString calName = cal.name;

        connect(reply, &QNetworkReply::finished, this, [this, reply, calName]() {
            reply->deleteLater();

            int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (reply->error() == QNetworkReply::NoError && status == 207) {
                QByteArray data = reply->readAll();
                auto tasks = ICalParser::parseTasks(data, calName);
                m_pendingTasks.append(tasks);
            } else {
                qWarning("CalDavService: skipping calendar %s: %s",
                         qPrintable(calName), qPrintable(reply->errorString()));
            }

            m_pendingCalendars--;
            if (m_pendingCalendars <= 0) {
                Q_EMIT tasksFetched(m_pendingTasks);
                m_pendingTasks.clear();
            }
        });
    }
}
