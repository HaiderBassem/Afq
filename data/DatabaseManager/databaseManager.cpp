#include "databaseManager.h"
#include "connectionPool.h"

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

std::unique_ptr<ConnectionWrapper> DatabaseManager::getConnection()
{
    return ConnectionPool::instance().getConnection();
}
