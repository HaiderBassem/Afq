#pragma once

#include <optional>
#include <QString>
#include <QVector>
#include <QMap>
#include <QJsonArray>
#include <QPair>
#include "DataModel/Person.h"

namespace DataAccess
{

class PeopleDataHandler
{
public:
    // CRUD Operations

    static std::optional<int> addPerson(const DataModel::Person& person);
    static std::optional<DataModel::Person> getPersonById(int personId);
    static QVector<DataModel::Person> getAllPeople();
    static bool updatePerson(const DataModel::Person& person);
    static bool deletePerson(int personId);

    // Search Operations
    // static std::optional<QVector<DataModel::Person>> getPeopleByName(const QString& fname,
    //                                                   const QString& sname = "",
    //                                                   const QString& tname = "",
    //                                                   const QString& ftname = "");

    // static QVector<DataModel::Person> getPeopleByGender(DataModel::Gender gender);
    // static QVector<DataModel::Person> getPeopleByBirthdateRange(const QDate& startDate,
    //                                                             const QDate& endDate);
    // static QVector<DataModel::Person> getPeopleByFullName(const QString& fullName);
    // static QVector<DataModel::Person> getPeopleByIds(const QList<int>& personIds);

    // Statistics
    static int getPeopleCount();
    static QPair<int, int> getGenderCount(); // male, female counts

    // Validation Checks
    static bool isPersonExist(int personId);
    static bool isPersonExist(const DataModel::Person& person);
    static bool isNameExist(const QString& fname, const QString& sname,
                            const QString& tname, const QString& ftname);
     // static bool isDuplicatePerson(const DataModel::Person& person);

    // Import/Export
    // static bool importFromJson(const QJsonArray& jsonArray);
    // static QJsonArray exportToJson(const QList<int>& personIds = QList<int>());
};

} // namespace DataAccess

