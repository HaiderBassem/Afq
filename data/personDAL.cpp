#include "personDAL.h"
#include "DatabaseManager/databaseManager.h"
#include "DatabaseManager/connectionWrapper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

namespace DataAccess
{

std::optional<int> Person::addPerson(
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
    ) {
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(
        INSERT INTO people(first_name, second_name, third_name, fourth_name, gender,
                           date_of_birth, address, phone_one, phone_two, email, image_path)
        VALUES(:fname, :sname, :tname, :foname, :gender,
               :dob, :address, :phone1, :phone2, :email, :img)
        RETURNING person_id
    )");

    query.bindValue(":fname", firstName);
    query.bindValue(":sname", secondName);
    query.bindValue(":tname", thirdName);
    query.bindValue(":foname", fourthName);
    query.bindValue(":gender", QString(gender));
    query.bindValue(":dob", dateOfBirth.toString("yyyy-MM-dd"));
    query.bindValue(":address", address);
    query.bindValue(":phone1", phoneOne);
    query.bindValue(":phone2", phoneTwo);
    query.bindValue(":email", email);
    query.bindValue(":img", imagePath);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    qWarning() << "addPerson error:" << query.lastError().text();
    return std::nullopt;
}

bool Person::updatePerson(
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
    ) {
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(
        UPDATE people
        SET first_name=:fname, second_name=:sname, third_name=:tname, fourth_name=:foname,
            gender=:gender, date_of_birth=:dob, address=:address,
            phone_one=:phone1, phone_two=:phone2, email=:email, image_path=:img
        WHERE person_id=:id
    )");

    query.bindValue(":fname", firstName);
    query.bindValue(":sname", secondName);
    query.bindValue(":tname", thirdName);
    query.bindValue(":foname", fourthName);
    query.bindValue(":gender", QString(gender));
    query.bindValue(":dob", dateOfBirth.toString("yyyy-MM-dd"));
    query.bindValue(":address", address);
    query.bindValue(":phone1", phoneOne);
    query.bindValue(":phone2", phoneTwo);
    query.bindValue(":email", email);
    query.bindValue(":img", imagePath);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "updatePerson error:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Person::deletePerson(int id) {
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("DELETE FROM people WHERE person_id=:id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "deletePerson error:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool Person::getPersonById(
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
    ) {
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT first_name, second_name, third_name, fourth_name, gender,
               date_of_birth, address, phone_one, phone_two, email, image_path
        FROM people
        WHERE person_id=:id
    )");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        firstName   = query.value(0).toString();
        secondName  = query.value(1).toString();
        thirdName   = query.value(2).toString();
        fourthName  = query.value(3).toString();
        gender      = query.value(4).toString().isEmpty() ? '\0' : query.value(4).toString().at(0).toLatin1();
        dateOfBirth = QDate::fromString(query.value(5).toString(), "yyyy-MM-dd");
        address     = query.value(6).toString();
        phoneOne    = query.value(7).toString();
        phoneTwo    = query.value(8).toString();
        email       = query.value(9).toString();
        imagePath   = query.value(10).toString();
        return true;
    }

    qWarning() << "getPersonById error:" << query.lastError().text();
    return false;
}

QVector<DataModel::Person> Person::getAllPeople() {
    QVector<DataModel::Person> people;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    if (!query.exec(R"(
        SELECT p.person_id, p.first_name, p.second_name, p.third_name, p.fourth_name,
               p.gender, p.date_of_birth, p.address, p.phone_one, p.phone_two,
               p.email, p.image_path, e.role, e.status
        FROM people p
        LEFT JOIN enrollment e ON p.person_id = e.person_id
    )")) {
        qWarning() << "getAllPeople error:" << query.lastError().text();
        return people;
    }

    while (query.next()) {
        DataModel::Person person;
        person.person_id   = query.value("person_id").toInt();
        person.first_name  = query.value("first_name").toString();
        person.second_name = query.value("second_name").toString();
        person.third_name  = query.value("third_name").toString();
        person.fourth_name = query.value("fourth_name").toString();
        person.gender      = query.value("gender").toString().isEmpty() ? '\0' : query.value("gender").toString().at(0).toLatin1();
        person.date_of_birth = QDate::fromString(query.value("date_of_birth").toString(), "yyyy-MM-dd");
        person.address     = query.value("address").toString();
        person.phone_one   = query.value("phone_one").toString();
        person.phone_two   = query.value("phone_two").toString();
        person.email       = query.value("email").toString();
        person.image_path  = query.value("image_path").toString();
        person.role        = query.value("role").toString();
        person.status      = query.value("status").toString();
        people.append(person);
    }

    return people;
}

QVector<DataModel::Person> Person::getPersonByName(
    const QString& firstName,
    const QString& secondName,
    const QString& thirdName,
    const QString& fourthName
    ) {
    QVector<DataModel::Person> people;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT person_id, first_name, second_name, third_name, fourth_name,
               gender, date_of_birth, address, phone_one, phone_two, email, image_path
        FROM people
        WHERE first_name=:fname AND second_name=:sname
          AND (:tname='' OR third_name=:tname)
          AND (:foname='' OR fourth_name=:foname)
    )");

    query.bindValue(":fname", firstName);
    query.bindValue(":sname", secondName);
    query.bindValue(":tname", thirdName);
    query.bindValue(":foname", fourthName);

    if (!query.exec()) {
        qWarning() << "getPersonByName error:" << query.lastError().text();
        return people;
    }

    while (query.next()) {
        DataModel::Person person;
        person.person_id   = query.value("person_id").toInt();
        person.first_name  = query.value("first_name").toString();
        person.second_name = query.value("second_name").toString();
        person.third_name  = query.value("third_name").toString();
        person.fourth_name = query.value("fourth_name").toString();
        person.gender      = query.value("gender").toString().isEmpty() ? '\0' : query.value("gender").toString().at(0).toLatin1();
        person.date_of_birth = QDate::fromString(query.value("date_of_birth").toString(), "yyyy-MM-dd");
        person.address     = query.value("address").toString();
        person.phone_one   = query.value("phone_one").toString();
        person.phone_two   = query.value("phone_two").toString();
        person.email       = query.value("email").toString();
        person.image_path  = query.value("image_path").toString();
        people.append(person);
    }

    return people;
}

bool Person::isPersonExist(int id) {
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM people WHERE person_id=:id LIMIT 1");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return true;
    }

    qWarning() << "isPersonExist error:" << query.lastError().text();
    return false;
}

} // namespace DataAccess
