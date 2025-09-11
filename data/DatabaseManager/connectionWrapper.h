#ifndef CONNECTION_WRAPPER_H
#define CONNECTION_WRAPPER_H

#include <QSqlDatabase>
#include <QString>

class ConnectionPool;

class ConnectionWrapper
{
public:
    ConnectionWrapper(const QString& connectionName, ConnectionPool* pool);
    ~ConnectionWrapper();

    QSqlDatabase database() const;

private:
    QString m_connectionName;
    ConnectionPool* m_pool;
};

#endif // CONNECTION_WRAPPER_H
