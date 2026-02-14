// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#ifndef TASKMODEL_H
#define TASKMODEL_H

#include "CalDavService.h"
#include "ICalParser.h"

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QTimer>

class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    // Config properties (bound from QML)
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString appPassword READ appPassword WRITE setAppPassword NOTIFY appPasswordChanged)
    Q_PROPERTY(int syncIntervalMinutes READ syncIntervalMinutes WRITE setSyncIntervalMinutes NOTIFY syncIntervalMinutesChanged)
    Q_PROPERTY(bool autoSync READ autoSync WRITE setAutoSync NOTIFY autoSyncChanged)

    // State properties (read-only from QML)
    Q_PROPERTY(bool syncing READ syncing NOTIFY syncingChanged)
    Q_PROPERTY(QString lastSyncTime READ lastSyncTime NOTIFY lastSyncTimeChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    enum Roles {
        UidRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        DueDateRole,
        PriorityRole,
        CompletedRole,
        CalendarNameRole,
    };
    Q_ENUM(Roles)

    explicit TaskModel(QObject *parent = nullptr);

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Config getters
    QString serverUrl() const;
    QString username() const;
    QString appPassword() const;
    int syncIntervalMinutes() const;
    bool autoSync() const;

    // Config setters
    void setServerUrl(const QString &url);
    void setUsername(const QString &user);
    void setAppPassword(const QString &password);
    void setSyncIntervalMinutes(int minutes);
    void setAutoSync(bool enabled);

    // State getters
    bool syncing() const;
    QString lastSyncTime() const;
    QString lastError() const;

    Q_INVOKABLE void sync();

Q_SIGNALS:
    // Config
    void serverUrlChanged();
    void usernameChanged();
    void appPasswordChanged();
    void syncIntervalMinutesChanged();
    void autoSyncChanged();

    // State
    void syncingChanged();
    void lastSyncTimeChanged();
    void lastErrorChanged();

    // Sync lifecycle
    void syncStarted();
    void syncSucceeded();
    void syncFailed(const QString &errorMessage);

private:
    void updateSyncTimer();
    void onTasksFetched(const QList<Task> &tasks);
    void onFetchError(const QString &message);

    // Config
    QString m_serverUrl;
    QString m_username;
    QString m_appPassword;
    int m_syncIntervalMinutes = 15;
    bool m_autoSync = true;

    // State
    bool m_syncing = false;
    QString m_lastSyncTime;
    QString m_lastError;

    // Data
    QList<Task> m_tasks;
    CalDavService m_service;
    QTimer m_syncTimer;
};

#endif // TASKMODEL_H
