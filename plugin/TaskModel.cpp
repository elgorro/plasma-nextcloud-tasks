// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2024 elgorro <dev@witiko.de>

#include "TaskModel.h"

#include <QDateTime>
#include <QLocale>

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&m_service, &CalDavService::tasksFetched, this, &TaskModel::onTasksFetched);
    connect(&m_service, &CalDavService::fetchError, this, &TaskModel::onFetchError);

    m_syncTimer.setTimerType(Qt::VeryCoarseTimer);
    connect(&m_syncTimer, &QTimer::timeout, this, &TaskModel::sync);
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_tasks.size();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tasks.size()) {
        return {};
    }

    const auto &task = m_tasks.at(index.row());

    switch (role) {
    case UidRole:
        return task.uid;
    case TitleRole:
        return task.title;
    case DescriptionRole:
        return task.description;
    case DueDateRole:
        return task.dueDate;
    case PriorityRole:
        return task.priority;
    case CompletedRole:
        return task.completed;
    case CalendarNameRole:
        return task.calendarName;
    }

    return {};
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    return {
        {UidRole, "uid"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {DueDateRole, "dueDate"},
        {PriorityRole, "priority"},
        {CompletedRole, "completed"},
        {CalendarNameRole, "calendarName"},
    };
}

// --- Config getters ---

QString TaskModel::serverUrl() const { return m_serverUrl; }
QString TaskModel::username() const { return m_username; }
QString TaskModel::appPassword() const { return m_appPassword; }
int TaskModel::syncIntervalMinutes() const { return m_syncIntervalMinutes; }
bool TaskModel::autoSync() const { return m_autoSync; }

// --- Config setters ---

void TaskModel::setServerUrl(const QString &url)
{
    if (m_serverUrl != url) {
        m_serverUrl = url;
        Q_EMIT serverUrlChanged();
        updateSyncTimer();
    }
}

void TaskModel::setUsername(const QString &user)
{
    if (m_username != user) {
        m_username = user;
        Q_EMIT usernameChanged();
    }
}

void TaskModel::setAppPassword(const QString &password)
{
    if (m_appPassword != password) {
        m_appPassword = password;
        Q_EMIT appPasswordChanged();
    }
}

void TaskModel::setSyncIntervalMinutes(int minutes)
{
    if (m_syncIntervalMinutes != minutes) {
        m_syncIntervalMinutes = minutes;
        Q_EMIT syncIntervalMinutesChanged();
        updateSyncTimer();
    }
}

void TaskModel::setAutoSync(bool enabled)
{
    if (m_autoSync != enabled) {
        m_autoSync = enabled;
        Q_EMIT autoSyncChanged();
        updateSyncTimer();
    }
}

// --- State getters ---

bool TaskModel::syncing() const { return m_syncing; }
QString TaskModel::lastSyncTime() const { return m_lastSyncTime; }
QString TaskModel::lastError() const { return m_lastError; }

// --- Sync ---

void TaskModel::sync()
{
    if (m_syncing || m_serverUrl.isEmpty()) {
        return;
    }

    m_syncing = true;
    Q_EMIT syncingChanged();

    m_lastError.clear();
    Q_EMIT lastErrorChanged();
    Q_EMIT syncStarted();

    m_service.fetchTasks(m_serverUrl, m_username, m_appPassword);
}

void TaskModel::onTasksFetched(const QList<Task> &tasks)
{
    beginResetModel();
    m_tasks = tasks;
    endResetModel();

    m_syncing = false;
    Q_EMIT syncingChanged();

    m_lastSyncTime = QLocale().toString(QDateTime::currentDateTime().time(),
                                        QLocale::ShortFormat);
    Q_EMIT lastSyncTimeChanged();
    Q_EMIT syncSucceeded();
}

void TaskModel::onFetchError(const QString &message)
{
    m_syncing = false;
    Q_EMIT syncingChanged();

    m_lastError = message;
    Q_EMIT lastErrorChanged();
    Q_EMIT syncFailed(message);
}

void TaskModel::updateSyncTimer()
{
    if (m_autoSync && !m_serverUrl.isEmpty()) {
        m_syncTimer.setInterval(m_syncIntervalMinutes * 60 * 1000);
        m_syncTimer.start();
        // triggeredOnStart equivalent: sync immediately
        sync();
    } else {
        m_syncTimer.stop();
    }
}
