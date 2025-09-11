#include "schoolDAL.h"
#include"DatabaseManager/databaseManager.h"
#include<QSqlQuery>
#include<QSqlError>

#include"dataModels.h"



std::optional<int> DataAccess::SchoolDAL::addSchool(const QString &name)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("INSERT INTO schools(name) VALUES(:name) RETURNING school_id");
    query.bindValue(":name", name);

    if(query.exec() && query.next())
        return query.value(0).toInt();


    //TODO: logging
    qWarning() << "SQL ERROR IN ADDSCHOOL:" << query.lastError().text();
    return std::nullopt;
}




bool DataAccess::SchoolDAL::updateSchool(const int &id, const QString &name)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("UPDATE schools SET name = :name WHERE school_id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);

    if(!query.exec())
    {    //TODO: logging
        qWarning() << "UPDATE SCHOOL ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool DataAccess::SchoolDAL::deleteSchool(const int &id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("DELETE FROM schools WHERE school_id = :id");
    query.bindValue(":id", id);

    if(!query.exec())
    {
        //TODO: logging
        qWarning() << "DELETE SCHOOL ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

std::optional<DataModel::School> DataAccess::SchoolDAL::getSchoolById(const int &id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT * FROM schools WHERE school_id = :id");
    query.bindValue(":id", id);

    if(query.exec() && query.next())
    {
        DataModel::School school;
        school.school_id = query.value("school_id").toInt();
        school.name      = query.value("name").toString();
        return school;
    }
        //TODO: logging
    qWarning() << "GET SCHOOL BY ID ERROR:" << query.lastError().text();
    return std::nullopt;
}

QVector<DataModel::School> DataAccess::SchoolDAL::getSchoolByName(const QString &name)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM schools WHERE name = :name");
    query.bindValue(":name",name);


    QVector<DataModel::School> schools;

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"GET SCHOOL BY NAME:" << query.lastError().text();
        return schools;
    }

    while(query.next())
    {
        DataModel::School school;
        school.school_id = query.value("school_id").toInt();
        school.name = query.value("name").toString();
        schools.append(school);
    }

    return schools;


}

bool DataAccess::SchoolDAL::isSchoolExist(const int &id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM schools WHERE school_id = :id LIMIT 1");
    query.bindValue(":id",id);
    if(query.exec() && query.next())
    {
        return true;
    }

    if (query.lastError().isValid())
        qWarning()<<"IS SCHOOL EXIST BY ID ERROR :"<<query.lastError().text();

    return false; // not exist
}

bool DataAccess::SchoolDAL::isSchoolExist(const QString &name)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM schools WHERE name = :name LIMIT 1");
    query.bindValue(":name", name);
    if(query.exec() && query.next())
        return true;

    if(query.lastError().isValid())
        qWarning() <<"IS SCHOOL EXIST BY NAME ERROR:" <<query.lastError().text();

    return false; // not exist

}

QVector<DataModel::School> DataAccess::SchoolDAL::getAllSchools()
{
    auto connWrappere = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrappere->database();

    QSqlQuery query(db);
    query.prepare("SELECT * FROM schools");


    QVector<DataModel::School> schools;

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"GET ALL SCHOOLS:" << query.lastError().text();
        return schools;
    }

    while(query.next())
    {
        DataModel::School school;
        school.school_id = query.value("school_id").toInt();
        school.name = query.value("name").toString();
        schools.append(school);
    }

    return schools;

}

std::optional<int> DataAccess::SchoolDAL::getSchoolCount()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM schools");
    if(query.exec() && query.next())
        return query.value(0).toInt();

    if(query.lastError().isValid())
        qWarning() << "GET SCHOOLS COUNT ERROR:"<<query.lastError().text();

    return std::nullopt;
}

QVector<DataModel::School> DataAccess::SchoolDAL::getSchoolsPaginated(int offset, int limit)
{
    auto connWrapper  = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);

    query.prepare("SELECT * FROM schools ORDER BY school_id LIMIT :limit OFFSET :offset");
    query.bindValue(":limit",limit);
    query.bindValue(":offset",offset);
    QVector<DataModel::School> schools;

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<< "GET SCHOOLS PAGINATED ERROR:"<<query.lastError().text();
        return schools;
    }

    while(query.next())
    {
        DataModel::School school;
        school.school_id = query.value("school_id").toInt();
        school.name = query.value("name").toString();
        schools.append(school);
    }
    return schools;
}

QVector<DataModel::School> DataAccess::SchoolDAL::searchSchoolsByPartialName(const QString &partialName)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT * FROM schools WHERE name ILIKE :partialName");
    query.bindValue(":partialName","%"+ partialName + "%");

    QVector<DataModel::School> schools;

    if(!query.exec())
    {
        qWarning() << "SEARCH SCHOOLS BY PARTIAL NAME ERROR:" <<query.lastError().text();
        return schools;
    }

    while(query.next())
    {
        DataModel::School school;
        school.school_id = query.value("school_id").toInt();
        school.name = query.value("name").toString();
        schools.append(school);
    }

    return schools;
}

bool DataAccess::SchoolDAL::deleteMultipleSchools(const QVector<int> &ids)
{
    if(ids.isEmpty()) return false;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);
    db.transaction();
    for (int id : ids)
    {
        query.prepare("DELETE FROM schools WHERE school_id = :id");
        query.bindValue(":id", id);
        if(!query.exec())
        {
            qWarning()<<"DELETE MULTIPLE SCHOOLS ERROR:"<<query.lastError().text();
            db.rollback(); // back
            return false;
        }
    }

    db.commit();
    return true;
}





