#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include <QSqlDatabase>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <memory>

class ConnectionWrapper;

class ConnectionPool
{
public:
    friend class ConnectionWrapper;

    static ConnectionPool& instance();

    std::unique_ptr<ConnectionWrapper> getConnection();

private:
    ConnectionPool(const QString& connName,
                   const QString& driver,
                   const QString& host,
                   int port,
                   const QString& dbName,
                   const QString& user,
                   const QString& password,
                   int poolSize = 5);

    void releaseConnection(const QString& connectionName);

    QString m_driver;
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_user;
    QString m_password;

    int m_poolSize;

    QQueue<QString> m_availableConnections;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
};

#endif // CONNECTION_POOL_H
