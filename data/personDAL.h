#ifndef PERSONDAL_H
#define PERSONDAL_H

#include <QDate>
#include <QString>
#include <QVector>
#include <optional>
#include "dataModels.h"

namespace DataAccess {

class Person
{
public:
    static std::optional<int> addPerson(
        const QString& firstName,
        const QString& secondName,
        const QString& thirdName,
        const QString& fourthName,
        const char& gender,
        const QDate& dateOfBirth,
        const QString& address,
        const QString& phoneOne,
        const QString& phoneTwo,
        const QString& email,
        const QString& imagePath
        );

    static bool updatePerson(
        const int& id,
        const QString& firstName,
        const QString& secondName,
        const QString& thirdName,
        const QString& fourthName,
        const char& gender,
        const QDate& dateOfBirth,
        const QString& address,
        const QString& phoneOne,
        const QString& phoneTwo,
        const QString& email,
        const QString& imagePath
        );

    static bool deletePerson(int id);

    static bool getPersonById(
        int id,
        QString& firstName,
        QString& secondName,
        QString& thirdName,
        QString& fourthName,
        char& gender,
        QDate& dateOfBirth,
        QString& address,
        QString& phoneOne,
        QString& phoneTwo,
        QString& email,
        QString& imagePath
        );

    static QVector<DataModel::Person> getPersonByName(
        const QString& firstName,
        const QString& secondName,
        const QString& thirdName = "",
        const QString& fourthName = ""
        );

    static QVector<DataModel::Person> getAllPeople();

    static bool isPersonExist(int id);
};

} // namespace DataAccess

#endif // PERSONDAL_H
