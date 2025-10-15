#include "PeopleDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"


#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>
#include<QVariant>
#include<QStringList>

#include<optional>
std::optional<int> DataAccess::PeopleDataHandler::addPerson(const DataModel::Person &person)
{
    if(person.first_name.isEmpty() || person.second_name.isEmpty() || person.third_name.isEmpty()
            || !person.date_of_birth.isValid())
    {
        qWarning() << "\033[33m Person name fields cannot be empty \033[0m";
        Logger::instance().warning("Person name fields cannot be empty");
        return std::nullopt;
    }

    if(!person.date_of_birth.isValid() || person.date_of_birth > QDate::currentDate().addYears(-6))
    {
        qWarning() << "\033[33m Invalid date of birth \033[0m";
        Logger::instance().error("Invalid date of birth");
        return std::nullopt;
    }

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return std::nullopt;
    }

    if(!db.transaction())
    {
        qWarning() << "\033[31m Cannot start transaction \033[0m";
        Logger::instance().error("Cannot start transaction");
        return std::nullopt;
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
        Logger::instance().error("Failed to add person " + query.lastError().text());
        db.rollback();
        return std::nullopt;
    }

    if(query.next())
    {
        int newId = query.value(0).toInt();
        Q_UNUSED(newId);
        qDebug() << "\033[32m Person added successfully with ID:" << newId << "\033[0m";
        Logger::instance().info("Person added successfully with ID:" + QString::number(newId));
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to commit" + query.lastError().text());
        db.rollback();
        return std::nullopt;
    }

    return query.value(0).toInt();
}

std::optional<DataModel::Person> DataAccess::PeopleDataHandler::getPersonById(int personId)
{
    if(personId <= 0)
    {
        qWarning()<< "\033[33m Invalid person ID\033[0m";
        Logger::instance().warning("Invalid person ID" + QString::number(personId));
        return std::nullopt;
    }

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return std::nullopt;
    }

    QSqlQuery query(db);
    query.prepare(R"(
                    SELECT person_id, first_name, second_name, third_name, fourth_name, gender, date_of_birth,
                            created_at, updated_at
                    FROM people
                    WHERE person_id = :person_id
)");

    query.bindValue(":person_id", personId);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get person \033[0m" <<query.lastError().text();
        Logger::instance().error("Failed to get person" + query.lastError().text());
        return std::nullopt;
    }

    try
    {
        DataModel::Person p;
        p.person_id = query.value("person_id").toInt();
        p.first_name = query.value("first_name").toString();
        p.second_name = query.value("second_name").toString();
        p.third_name = query.value("third_name").toString();
        p.fourth_name = query.value("fourth_name").toString();
        p.gender = DataModel::charToGender(query.value("gender").toChar());
        p.date_of_birth = query.value("date_of_birth").toDate();
        p.created_at = query.value("created_at").toDateTime();
        p.updated_at = query.value("updated_at").toDateTime();

        qDebug()<<"\033[32m Successfully retrieved person with ID: \033[0m" <<personId;
        return p;
    }
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31m ERROR processing person data: \033[0m" <<e.what();
        Logger::instance().error("ERROR processing person data:" + QString(e.what()));
        return std::nullopt;
    }
}

QVector<DataModel::Person> DataAccess::PeopleDataHandler::getAllPeople()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.transaction())
    {
        qWarning() <<"\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return QVector<DataModel::Person>();
    }

    QSqlQuery query(db);
    query.prepare(R"(
                        SELECT person_id, first_name, second_name, third_name, fourth_name,
                               gender, date_of_birth, created_at, updated_at
                        FROM people
                        ORDER BY person_id
                )");

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get people \033[0m"<<query.lastError().text();
        Logger::instance().error("Failed to get people" + query.lastError().text());
        return QVector<DataModel::Person>();
    }

    QVector<DataModel::Person> people;
    while(query.next())
    {
        try
        {
        DataModel::Person p;
        p.person_id = query.value("person_id").toInt();
        p.first_name = query.value("first_name").toString();
        p.second_name = query.value("second_name").toString();
        p.third_name = query.value("third_name").toString();
        p.fourth_name = query.value("fourth_name").toString();
        p.gender = DataModel::charToGender(query.value("gender").toChar());
        p.date_of_birth = query.value("date_of_birth").toDate();
        p.created_at = query.value("created_at").toDateTime();
        p.updated_at = query.value("updated_at").toDateTime();
        people.append(p);
        }
        catch(const std::exception& e)
        {
            qCritical() << "\033[31m Error processing person data: \033[0m" << e.what();
            Logger::instance().error("Error processing person data: " + QString(e.what()));
        }

    }
    qDebug() << "\033[32m Successfully retrieved \033[0m" << people.size() <<"\033[32m people \033[0m";
    return people;

}

bool DataAccess::PeopleDataHandler::updatePerson(const DataModel::Person &person)
{
    if(person.person_id <= 0)
    {
        qWarning() <<"\033[33m Invalid person ID for update \033[0m";
        Logger::instance().warning("Invalid person ID for update");
        return false;
    }

    if(person.first_name.isEmpty() || person.second_name.isEmpty() || person.third_name.isEmpty() || !person.date_of_birth.isValid())
    {
        qWarning()<<"\033[33m Person data is invalid for update \033[0m";
        Logger::instance().error("Person data is invalid for update");
        return false;
    }

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qWarning()<<"\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Databse connection is not open");
        return false;
    }

    if(!db.transaction())
    {
        qWarning() <<"\033[31m Cannot start transaction \033[0m";
        Logger::instance().error("Cannot start transaction");
        return false;
    }

    QSqlQuery query(db);

    query.prepare(R"(
                    UPDATE people
                    SET first_name = :fname, second_name = :sname, third_name = :tname, fourth_name = :ftname,
                    gender = :g, date_of_birth = :dob,
                    update_at = NOW()
                    WHERE person_id =:id
    )");

    query.bindValue(":id", person.person_id);
    query.bindValue(":fname", person.first_name);
    query.bindValue(":sname", person.second_name);
    query.bindValue(":tname", person.third_name);
    query.bindValue(":ftname", person.fourth_name);
    query.bindValue(":g", DataModel::genderToString(person.gender));
    query.bindValue(":dob", person.date_of_birth);


    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to update person \033[0m" <<query.lastError().text();
        Logger::instance().error("Failed to update person" + query.lastError().text());
        return false;
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to update commit \033[0m"<<db.lastError().text();
        Logger::instance().error("Failed to update commit" + db.lastError().text());
        return false;
    }

    qInfo() << "\033[32m Successfully updated person with ID: \033[0m" <<person.person_id;
    Logger::instance().info("Successfully updated person with ID:" + QString::number(person.person_id));
    return true;
}

bool DataAccess::PeopleDataHandler::deletePerson(int personId)
{
    if(personId <= 0)
    {
        qWarning() << "\033[33m Invalid person ID for deletion \033[0m";
        Logger::instance().warning("Invalid person ID for deletion");
        return false;
    }

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if (!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return false;
    }


    if (!db.transaction())
    {
        qWarning() << "\033[31m Cannot start transaction \033[0m";
        Logger::instance().error("Cannot start transaction");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM people WHERE person_id = :id");
    query.bindValue(":id", personId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to delete person \033[0m"<<query.lastError().text();
        Logger::instance().error("Failed to delete person" + query.lastError().text());
        db.rollback();
        return false;
    }

    if(query.numRowsAffected() <= 0)
    {
        qWarning() << "\033[33m No person found with ID: \033[0m"<<personId;
        Logger::instance().warning("No person found with ID: " + QString::number(personId));
        db.rollback();
        return false;
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit" + db.lastError().text());
        db.rollback();
        return false;
    }

    qInfo() << "\033[32m Successfully delete person with ID: \033[0m"<< personId;
    Logger::instance().info("Successfully delete person with ID: " + QString::number(personId));

    return true;
}

int DataAccess::PeopleDataHandler::getPeopleCount()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if (!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return false;
    }
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM people");

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get people count \033[0m " << query.lastError().text();
        Logger::instance().error("Failed to get people count" + query.lastError().text());
        return 0;
    }

    if(query.next())
    {
        qDebug() << "\033[32m Total people count: " <<query.value(0).toString();
        return query.value(0).toInt();
    }


    qWarning() <<"\033[33m No count result returned \033[0m ";
    Logger::instance().warning("No count result returned ");
    return 0;
}

QPair<int, int> DataAccess::PeopleDataHandler::getGenderCount()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if (!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m";
        Logger::instance().error("Database connection is not open");
        return QPair<int, int>(0,0);
    }

    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT COUNT(CASE WHEN gender IN('M', 'm') THEN 1 END)   AS Mgender,
                        COUNT(CASE WHEN gender IN('F', 'f') THEN 1 END )  AS Fgender
                    FROM people
            )")    ;

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get people by gender \033[0m" + query.lastError().text();
        Logger::instance().error("Failed to get people by gender " + query.lastError().text());
        return QPair<int, int>(0,0);
    }
    QPair<int, int> pr;
    if(query.next())
    {
        pr.first = query.value("Mgender").toInt();
        pr.second = query.value("Fgender").toInt();
        qDebug() <<"Male: " << QString::number(pr.first)+" ";
        qDebug() <<"Female: " << QString::number(pr.second) + " ";
        return pr;
    }



    qWarning() << "\033[33m No count result returned \033[0m";
    Logger::instance().warning("No count result returned ");
    return QPair<int, int>(0,0);
}

bool DataAccess::PeopleDataHandler::isPersonExist(int personId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database is not open\033[0m";
        Logger::instance().error("Database is not open" + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare("SELECT 1 FROM people WHERE person_id = ? LIMIT 1");
    query.addBindValue(personId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to check person existence:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to check person existence" + query.lastError().text());
        return false;
    }

    return query.next();
}



bool DataAccess::PeopleDataHandler::isPersonExist(const DataModel::Person &person)
{
    if(person.first_name.trimmed().isEmpty() && person.second_name.trimmed().isEmpty() && person.third_name.trimmed().isEmpty() && person.fourth_name.trimmed().isEmpty() || !person.date_of_birth.isValid())
    {
        qWarning() << "\033[31m You must type the full name, and check the date of birth \033[0m";
        Logger::instance().warning("You must type the full name, and check the date of birth");
        return false;
    }
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open \033[0m" + db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.prepare(R"(
                    SELECT EXISTS(
                                    SELECT 1 FROM people
                                    WHERE first_name = :fname
                                            AND second_name = :sname
                                            AND third_name = :tname
                                            AND fourth_name = :ftname
                                            AND date_of_birth = :dob
)
    )");

    QVector<QPair<QString, QVariant>> bindValues;

    bindValues.append({":fname", person.first_name.trimmed()});
    bindValues.append({":sname", person.second_name.trimmed()});
    bindValues.append({":tname", person.third_name});
    bindValues.append({":ftname", person.fourth_name});
    bindValues.append({":dob", person.date_of_birth});

    for(const auto& bindPair : bindValues)
        query.bindValue(bindPair.first, bindPair.second);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to check person existence \033[0m" + query.lastError().text();
        Logger::instance().error("Failed to check person existence " + query.lastError().text());
        return false;
    }

    if(query.next())

    return      query.value(0).toBool();


    qWarning() << "\033[33m No result returned when checking person existence \033[0m" ;
    Logger::instance().warning("No result returned when checking person existence ");
    return false;



}




bool DataAccess::PeopleDataHandler::isNameExist(const QString &fname, const QString &sname, const QString &tname, const QString &ftname)
{
    if(fname.trimmed().isEmpty())
        return false;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    if(!db.isOpen())
    {
        qWarning() << "\033[31m Database connection is not open \033[0m" + db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    QStringList conditions;
    QVector<QPair<QString, QVariant>> bindValues;

    conditions << "first_name = :fname";
    bindValues << qMakePair(QString(":fname"), fname.trimmed());


    if(!sname.trimmed().isEmpty())
    {
        conditions <<"second_name = :sname";
        bindValues <<qMakePair(":sname", sname.trimmed());

    }

    if(!tname.trimmed().isEmpty())
    {
        conditions << "third_name = :tname";
        bindValues << qMakePair(":tname", tname.trimmed());
    }
    if(!ftname.trimmed().isEmpty())
    {
        conditions << "fourth_name = :ftname";
        bindValues << qMakePair(":ftname", ftname.trimmed());
    }

    QString queryString = QString("SELECT EXISTS(SELECT 1 FROM people WHERE %1 LIMIT 1")
                              .arg(conditions.join(" AND "));


    QSqlQuery query(db);
    query.prepare(queryString);

    for(const auto& bindPair : bindValues)
        query.bindValue(bindPair.first, bindPair.second);


    if(query.exec())
        return query.value(0).toBool();

    qWarning() << "\033[33m No result returned when checking name existence \033[0m";
    Logger::instance().warning("No result returned when checking name existence ");
    return false;
}
















































