#include "connectionPool.h"
#include "connectionWrapper.h"
#include <QSqlDatabase>
#include <QUuid>
#include <QDebug>

#include <QtSql/QSqlError>

ConnectionPool& ConnectionPool::instance()
{
    static ConnectionPool pool(
        "schooldb_connection",
        "QPSQL",          // PostgreSQL driver
        "127.0.0.1",      // host
        5432,             // port
        "schooldb",       // dbname
        "haider",         // user
        "0770",           // password
        5                 // pool size
        );
    return pool;
}

ConnectionPool::ConnectionPool(const QString& connName,
                               const QString& driver,
                               const QString& host,
                               int port,
                               const QString& dbName,
                               const QString& user,
                               const QString& password,
                               int poolSize)
    : m_driver(driver),
    m_host(host),
    m_port(port),
    m_dbName(dbName),
    m_user(user),
    m_password(password),
    m_poolSize(poolSize)
{
    // Initialize pool with N connections
    for (int i = 0; i < m_poolSize; ++i) {
        QString connectionName = QString("%1_%2")
        .arg(connName)
            .arg(QUuid::createUuid().toString());
        QSqlDatabase db = QSqlDatabase::addDatabase(m_driver, connectionName);
        db.setHostName(m_host);
        db.setPort(m_port);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_user);
        db.setPassword(m_password);

        if (!db.open())
        {
            //TODO: logging
            qWarning() << "Failed to open DB connection:" << db.lastError().text();
        }

        m_availableConnections.enqueue(connectionName);
    }
}

void ConnectionPool::releaseConnection(const QString& connectionName)
{
    QMutexLocker locker(&m_mutex);
    m_availableConnections.enqueue(connectionName);
    m_waitCondition.wakeOne();
}

std::unique_ptr<ConnectionWrapper> ConnectionPool::getConnection()
{
    QMutexLocker locker(&m_mutex);

    while (m_availableConnections.isEmpty()) {
        m_waitCondition.wait(&m_mutex);
    }

    QString connectionName = m_availableConnections.dequeue();
    return std::make_unique<ConnectionWrapper>(connectionName, this);
}
