#include "connectionWrapper.h"
#include "connectionPool.h"

ConnectionWrapper::ConnectionWrapper(const QString& connectionName, ConnectionPool* pool)
    : m_connectionName(connectionName), m_pool(pool)
{
}

ConnectionWrapper::~ConnectionWrapper()
{
    if (!m_connectionName.isEmpty() && m_pool) {
        m_pool->releaseConnection(m_connectionName);
    }
}

QSqlDatabase ConnectionWrapper::database() const
{
    return QSqlDatabase::database(m_connectionName);
}
