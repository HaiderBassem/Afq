#include "ClassDAL.h"


#include"DatabaseManager/databaseManager.h"
#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>
#include<optional>

std::optional<int> ClassDAL::createClass(int schoolId, const QString &name, const QString &section, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("INSERT INTO classes (schoolId, name, section, year) VALUES (:schoolId, :name, :section, :year) RETURNING class_id ");
    query.bindValue(":schoolId",schoolId);
    query.bindValue(":name",name);
    query.bindValue(":section",section);
    query.bindValue(":year",year);

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"CREATE CLASS ERROR:"<<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
        return query.value(0).toInt();

    return std::nullopt;
}

std::optional<DataModel::Class> ClassDAL::getClassById(int classId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE class_id = :classId LIMIT 1");
    query.bindValue(":classId",classId);

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"GET CLASS BY ID ERROR:"<<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year = query.value("year").toInt();
        return cls;
    }
    return std::nullopt;
}



bool ClassDAL::updateClass(int classId, const QString &name, const QString &section, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE classes SET name = :name, section = :section, year = :year WHERE class_id = :classId ");

    query.bindValue(":classId",classId);
    query.bindValue(":name",name);
    query.bindValue(":section",section);
    query.bindValue(":year",year);

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"UPDATE CLASS ERROR:"<<query.lastError().text();
        return false;
    }
        return query.numRowsAffected() > 0;

}

bool ClassDAL::deleteClass(int classId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("DELETE FROM classes WHERE class_id = :classId ");
    query.bindValue(":classId", classId);

    if(!query.exec())
    {
        //TODO: logging
        qWarning()<<"DELETE CLASS ERROR:"<<query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;

}

QVector<DataModel::Class> ClassDAL::getClassesBySchool(int schoolId)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE school_id = :schoolId ");
    query.bindValue(":schoolId", schoolId);

    if (!query.exec())
    {
        qWarning() << "GET CLASSES BY SCHOOL ERROR:" << query.lastError().text();
        return Classes;
    }


    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year = query.value("year").toInt();
        Classes.append(cls);
    }
    return Classes;

}

QVector<DataModel::Class> ClassDAL::getClassesByYear(int year)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE year = :year ");
    query.bindValue(":year", year);

    if (!query.exec())
    {
        qWarning() << "GET CLASSES BY YEAR ERROR:" << query.lastError().text();
        return Classes;
    }


    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year = query.value("year").toInt();
        Classes.append(cls);
    }
    return Classes;
}

QVector<DataModel::Class> ClassDAL::searchClasses(const QString &searchTerm)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE name ILIKE :term OR section ILIKE :term ");
    query.bindValue(":term", "%"+ searchTerm +"%");
    if(!query.exec())
    {
        qWarning()<<"SEARCH CLASSES ERROR:" << query.lastError().text();
        return Classes;
    }
    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year = query.value("year").toInt();
        Classes.append(cls);
    }
    return Classes;
}

QVector<DataModel::Class> ClassDAL::getAllClasses(int limit, int offset)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes OFFSET :offset LIMIT :limit");
    query.bindValue(":offset",offset);
    query.bindValue(":limit",limit);

    if(!query.exec())
    {
        qWarning()<<"GET ALL CLASSES ERROR:" << query.lastError().text();
        return Classes;
    }
    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year = query.value("year").toInt();
        Classes.append(cls);
    }
    return Classes;

}



