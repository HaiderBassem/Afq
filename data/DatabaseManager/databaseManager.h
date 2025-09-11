#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <memory>
#include "connectionWrapper.h"

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    std::unique_ptr<ConnectionWrapper> getConnection();

private:
    DatabaseManager() = default;
};

#endif // DATABASE_MANAGER_H
