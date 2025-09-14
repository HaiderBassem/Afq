#include "SchoolDAL.h"
#include"DatabaseManager/databaseManager.h"
#include<QSqlQuery>
#include<QSqlError>

#include"DataModels.h"



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

    if(!query.exec())
        {
        qWarning() << "GET SCHOOL BY ID EXECUTION ERROR:" << query.lastError().text();
        return std::nullopt;
    }

    if(!query.next())
    {
        return std::nullopt;
    }

    DataModel::School school;
    school.school_id = query.value("school_id").toInt();
    school.name = query.value("name").toString();
    return school;
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
    if(ids.isEmpty()) return true;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QString placeholders;
    for(int i = 0; i < ids.size(); ++i) {
        placeholders += (i > 0) ? ",?" : "?";
    }

    QSqlQuery query(db);
    query.prepare(QString("DELETE FROM schools WHERE school_id IN (%1)").arg(placeholders));

    for(int id : ids) {
        query.addBindValue(id);
    }

    if(!query.exec()) {
        qWarning() << "DELETE MULTIPLE SCHOOLS ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

std::optional<int> DataAccess::SchoolDAL::getActiveStudentsCount(int schoolID)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM enrollment WHERE school_id = :schoolID AND role ='Student' AND status ='Active' ");
    query.bindValue(":schoolID",schoolID);

    if(!query.exec())
    {
        qWarning()<<"GET ACTIVE SYUDENTS COUNT ERROR:"<<query.lastError().text();
        return std::nullopt;
    }
    if(query.next())
    {
        return query.value(0).toInt();
    }

    return 0;
}

std::optional<int>  DataAccess::SchoolDAL::getActiveTeacherCount(int schoolID)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM enrollment WHERE school_id = :schoolID AND role = 'Teacher' And status = 'Active' ");
    query.bindValue(":schoolID", schoolID);

    if(!query.exec())
    {
        qWarning()<<"GET ACTIVE TEACHERS COUNT ERROR:"<<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
        return query.value(0).toInt();

    return 0;
}

std::optional<int> DataAccess::SchoolDAL::getClassesCount(int schoolID, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);
    query.prepare(R"(SELECT COUNT(*)
                    FROM classes WHERE school_id = :schoolID
                    AND year = :year )");

    query.bindValue(":schooID", schoolID);
    query.bindValue(":year", year);

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<< "GET CLASSES COUNT ERROR:"<<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
        return query.value(0).toInt();
    return 0;
}








