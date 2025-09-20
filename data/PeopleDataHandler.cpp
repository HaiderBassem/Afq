#include "PeopleDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"


#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
#include<QVariant>


bool DataAccess::PeopleDataHandler::addPerson(const DataModel::Person &person)
{
    if(person.first_name.isEmpty() || person.second_name.isEmpty() || person.third_name.isEmpty()
            || !person.date_of_birth.isValid())
    {
        qWarning() << "\033[33m Person name fields cannot be empty \033[0m";
        Logger::instance().log("Person name fields cannot be empty");
        return false;
    }

    if(!person.date_of_birth.isValid() || person.date_of_birth > QDate::currentDate().addYears(-6))
    {
        qWarning() << "\033[33m Invalid date of birth \033[0m";
        Logger::instance().log("Invalid date of birth");
        return false;
    }

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().log("Database connection is not open");
        return false;
    }

    if(!db.transaction())
    {
        qWarning() << "\033[31m Cannot start transaction \033[0m";
        Logger::instance().log("Cannot start transaction");
        return false;
    }

    QSqlQuery query(db);


    query.prepare(R"(
                    INSERT INTO people
                        (first_name, second_name, third_name, fourth_name, gender, date_of_birth)
                    VALUES (:fname, :sname, :tname, :ftname, :g, :dob)
                    RETURNING person_id
)");

    query.bindValue(":fname",person.first_name.trimmed());
    query.bindValue(":sname", person.second_name.trimmed());
    query.bindValue(":tname", person.third_name.trimmed());
    query.bindValue(":ftname", person.fourth_name.trimmed());
    query.bindValue(":g", QString(QChar(static_cast<char>(person.gender))));
    query.bindValue(":dob", person.date_of_birth);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to add person\033[0m" << query.lastError().text();
        Logger::instance().log("Failed to add person" + query.lastError().text());
        db.rollback();
        return false;
    }

    if(query.next())
    {
        int newId = query.value(0).toInt();
        Q_UNUSED(newId);
        qDebug() << "\033[32m Person added successfully with ID:" << newId << "\033[0m";
        Logger::instance().log("Person added successfully with ID:" + QString::number(newId));
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit\033[0m" << query.lastError().text();
        Logger::instance().log("Failed to commit" + query.lastError().text());
        db.rollback();
        return false;
    }

    return true;
}











































