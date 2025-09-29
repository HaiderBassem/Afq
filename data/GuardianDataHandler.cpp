#include "GuardianDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"
#include"data/PeopleDataHandler.h"


#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>


bool DataAccess::GuardianDataHandler::addGuardian(const DataModel::Guardian &guardian)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open \033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open" + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() <<"\033[31m Failed to start transaction \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to start transaction " + db.lastError().text());
        return false;
    }

    //check this person have not Guardian formaly
    QSqlQuery checkGuardianQuery(db);
    checkGuardianQuery.prepare("SELECT guardian_id FROM guardians WHERE person_id = ? LIMIT 1");
    checkGuardianQuery.addBindValue(guardian.person_id);

    if(checkGuardianQuery.exec() && checkGuardianQuery.next())
    {
        qCritical() <<"\033[31m Guardian already exists for person ID: " << guardian.person_id;
        db.rollback();
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare(R"(
                            INSERT INTO guardians(person_id, occupation, phone_number, education_level)
                            VALUES (?, ?, ?, ?)
                            RETURNING guardian_id
)");
    insertQuery.addBindValue(guardian.person_id);
    insertQuery.addBindValue(guardian.occupation.trimmed());
    insertQuery.addBindValue(guardian.work_phone.trimmed());
    insertQuery.addBindValue(guardian.education_level.trimmed());

    if(!insertQuery.exec()  || !insertQuery.next())
    {
        qCritical() <<"\033[31m Failed to add guardian: \033[0m"  << insertQuery.lastError().text();
        Logger::instance().error("Failed to add guardian: " + insertQuery.lastError().text());
        db.rollback();
        return false;
    }

    int newGuardianId = insertQuery.value("guardian_id").toInt();

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction " << db.lastError().text();
        Logger::instance().error("Failed to commit transaction " + db.lastError().text());
        db.rollback();
        return false;
    }

    qInfo() <<"\033[32m Successfully added guardian with ID: \033[0m" << newGuardianId ;
    return true;

}
bool DataAccess::GuardianDataHandler::deleteGuardian(int guardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction.\033[0m" << db.lastError().text();
        Logger::instance().error("Failed to start transaction: " + db.lastError().text());
        return false;
    }

    QSqlQuery checkQuery(db);
    checkQuery.prepare(R"(
        SELECT guardian_id, person_id
        FROM guardians
        WHERE guardian_id = ?
    )");
    checkQuery.addBindValue(guardianId);

    if(!checkQuery.exec() || !checkQuery.next())
    {
        qCritical() << "\033[31m Guardian not found with ID:\033[0m" << guardianId;
        Logger::instance().error("Guardian not found with ID: " + QString::number(guardianId));
        db.rollback();
        return false;
    }

    int personId = checkQuery.value("person_id").toInt();

    QSqlQuery relationshipQuery(db);
    relationshipQuery.prepare("SELECT COUNT(*) AS relation_count FROM student_guardians WHERE guardian_id = ?");
    relationshipQuery.addBindValue(guardianId);

    int relationCount = 0;
    if(relationshipQuery.exec() && relationshipQuery.next())
    {
        relationCount = relationshipQuery.value("relation_count").toInt();
    }

    if(relationCount > 0)
    {
        QSqlQuery deleteRelationQuery(db);
        deleteRelationQuery.prepare("DELETE FROM student_guardians WHERE guardian_id = ?");
        deleteRelationQuery.addBindValue(guardianId);

        if(!deleteRelationQuery.exec())
        {
            qCritical() << "\033[31m Failed to delete guardian-student relationships for guardian ID:\033[0m" << guardianId;
            Logger::instance().error("Failed to delete guardian-student relationships for guardian ID: " + QString::number(guardianId));
            db.rollback();
            return false;
        }

        qInfo() << "\033[33m Deleted" << relationCount << "student relationships for guardian ID:\033[0m" << guardianId;
    }

    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM guardians WHERE guardian_id = ?");
    deleteQuery.addBindValue(guardianId);

    if(!deleteQuery.exec())
    {
        qCritical() << "\033[31m Failed to delete guardian ID:\033[0m" << guardianId
                    << "Error:" << deleteQuery.lastError().text();
        Logger::instance().error("Failed to delete guardian ID: " + QString::number(guardianId) +
                                 " Error: " + deleteQuery.lastError().text());
        db.rollback();
        return false;
    }

    if(deleteQuery.numRowsAffected() == 0)
    {
        qWarning() << "\033[33m No rows affected when deleting guardian ID:\033[0m" << guardianId;
        Logger::instance().warning("No rows affected when deleting guardian ID: " + QString::number(guardianId));
        db.rollback();
        return false;
    }

    if(!db.commit())
    {
        qCritical() << "\033[31m Failed to commit transaction.\033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
        db.rollback();
        return false;
    }

    qInfo() << "\033[32m Successfully deleted guardian with ID:\033[0m" << guardianId
            << "(Person ID:" << personId << ")";

    return true;
}

DataModel::Guardian DataAccess::GuardianDataHandler::getGuardianById(int guardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open. " << db.lastError().text();
        Logger::instance().error("Database connection is not open. " + db.lastError().text());
        return DataModel::Guardian();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                    SELECT  g.guardian_id,
                            g.person_id,
                            g.occupation,
                            g.phone_number,
                            g.education_level,
                            p.first_name,
                            p.second_name,
                            p.third_name,
                            p.fourth_name,
                            p.gender,
                            p.date_of_birth
                    FROM guardians g
                    JOIN people p ON p.person_id = g.person_id
                    WHERE g.guardian_id = ?
)");

    query.addBindValue(guardianId);

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get guardian by ID: \033[0m" << guardianId << " " << query.lastError().text();
        Logger::instance().error("Failed to ger guardian by ID " +query.lastError().text());
        return DataModel::Guardian();
    }

    DataModel::Guardian guardian;

    if(query.next())
    {
        guardian.guardian_id = query.value("guardian_id").toInt();
        guardian.person_id = query.value("person_id").toInt();
        guardian.occupation = query.value("occupation").toString();
        guardian.work_phone = query.value("phone_number").toString();
        guardian.education_level = query.value("education_level").toString();

        // person info
        guardian.first_name = query.value("first_name").toString();
        guardian.second_name = query.value("second_name").toString();
        guardian.third_name = query.value("third_name").toString();
        guardian.fourth_name = query.value("fourth_name").toString();
        guardian.gender = query.value("gender").toChar();
        guardian.date_of_birth = query.value("date_of_birth").toDate();

        qDebug() <<"\033[32m Data Retrieved for: \033[0m" << guardian.full_name;
        return guardian;
    }
    qWarning() <<"\033[33m Guardian not found with ID: \033[0m" << guardianId;
    return guardian;
}

QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::getAllGuardiansById()
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database connection is not open. " << db.lastError().text();
        Logger::instance().error("Database connection is not open. " + db.lastError().text());
        return QVector< DataModel::Guardian>();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                    SELECT  g.guardian_id,
                            g.person_id,
                            g.occupation,
                            g.phone_number,
                            g.education_level,
                            p.first_name,
                            p.second_name,
                            p.third_name,
                            p.fourth_name,
                            p.gender,
                            p.date_of_birth
                    FROM guardians g
                    JOIN people p ON p.person_id = g.person_id
                    ORDER BY g.guardian_id ASC
)");

    if(!query.exec())
    {
        qCritical() <<"\033[31m Failed to get all guardians \033[0m" << " " << query.lastError().text();
        Logger::instance().error("Failed to get all guardians " +query.lastError().text());
        return QVector< DataModel::Guardian>();
    }


    QVector< DataModel::Guardian>  guardians;
    if(query.next())
    {
        DataModel::Guardian guardian;
        guardian.guardian_id = query.value("guardian_id").toInt();
        guardian.person_id = query.value("person_id").toInt();
        guardian.occupation = query.value("occupation").toString();
        guardian.work_phone = query.value("phone_number").toString();
        guardian.education_level = query.value("education_level").toString();

        // people info
        guardian.first_name = query.value("first_name").toString();
        guardian.second_name = query.value("second_name").toString();
        guardian.third_name = query.value("third_name").toString();
        guardian.fourth_name = query.value("fourth_name").toString();
        guardian.gender = query.value("gender").toChar();
        guardian.date_of_birth = query.value("date_of_birth").toDate();

        qDebug() <<"\033[32m Data Retrieved for: \033[0m" << guardian.full_name;
         guardians.append(guardian);
    }

     return guardians;
}
bool DataAccess::GuardianDataHandler::addStudentGuardianRelationship(int studentPersonId, int guardianId, const QString &relationship, bool isPrimary)
{

    if(!PeopleDataHandler::isPersonExist(studentPersonId) || !isGuardianExists(guardianId))
        return false;

    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction.\033[0m" << db.lastError().text();
        Logger::instance().error("Failed to start transaction: " + db.lastError().text());
        return false;
    }

    QSqlQuery checkRelationship(db);
    checkRelationship.prepare("SELECT student_guardian_id FROM student_guardians WHERE student_person_id = ? AND guardian_id = ?");
    checkRelationship.addBindValue(studentPersonId);
    checkRelationship.addBindValue(guardianId);

    if(checkRelationship.exec() && checkRelationship.next())
    {
        qCritical() << "\033[31m Relationship already exists between student ID: \033[0m" << studentPersonId << " and guardian ID: " << guardianId;
        Logger::instance().error("Relationship already exists between student and guardian");
        db.rollback();
        return false;
    }

    if(isPrimary)
    {
        QSqlQuery updatePrimaryQuery(db);
        updatePrimaryQuery.prepare("UPDATE student_guardians SET is_primary = false WHERE student_person_id = ?");
        updatePrimaryQuery.addBindValue(studentPersonId);

        if(!updatePrimaryQuery.exec())
        {
            qWarning() << "\033[33m Failed to update existing primary relationship for student ID: \033[0m" << studentPersonId << " " << updatePrimaryQuery.lastError().text();
            Logger::instance().warning("Failed to update existing primary relationship for student " + updatePrimaryQuery.lastError().text());
            // do not return false, it's not dangerous case
        }
    }


    QSqlQuery insertQuery(db);
    insertQuery.prepare(R"(
        INSERT INTO student_guardians (student_person_id, guardian_id, relationship, is_primary)
        VALUES (?, ?, ?, ?)
        RETURNING student_guardian_id
    )");

    insertQuery.addBindValue(studentPersonId);
    insertQuery.addBindValue(guardianId);
    insertQuery.addBindValue(relationship.trimmed());
    insertQuery.addBindValue(isPrimary);

    if(!insertQuery.exec() || !insertQuery.next())
    {
        qCritical() << "\033[31m Failed to add student-guardian relationship: \033[0m" << insertQuery.lastError().text();
        Logger::instance().error("Failed to add student-guardian relationship: " + insertQuery.lastError().text());
        db.rollback();
        return false;
    }

    int newRelationshipId = insertQuery.value("student_guardian_id").toInt();

    if(!db.commit())
    {
        qCritical() << "\033[31m Failed to commit transaction.\033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
        db.rollback();
        return false;
    }

    qInfo() << "\033[32m Successfully added student-guardian relationship ID:\033[0m" << newRelationshipId
            << "Student ID:" << studentPersonId
            << "Guardian ID:" << guardianId
            << "Relationship:" << relationship
            << "Is Primary:" << (isPrimary ? "Yes" : "No");

    return true;
}

bool DataAccess::GuardianDataHandler::removeStudentGuardianRelationship(int studentGuardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }


    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction.\033[0m" << db.lastError().text();
        Logger::instance().error("Failed to start transaction: " + db.lastError().text());
        return false;
    }
    // check the relationship is exist
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT 1 FROM student_guardians WHERE student_guardian_id = ? LIMIT 1 ");
    checkQuery.addBindValue(studentGuardianId);

    if(!checkQuery.exec() || !checkQuery.next())
    {
        qCritical() << "\033[31m Student-guardian relationship not found with ID:\033[0m" << studentGuardianId;
        Logger::instance().error("Student-guardian relationship not found with ID: " + QString::number(studentGuardianId));
        db.rollback();
        return false;
    }


    //get the data before delete it // juut for logging


    QSqlQuery infoQuery(db);
    infoQuery.prepare(R"(
                        SELECT sg.student_person_id

)"


}

bool DataAccess::GuardianDataHandler::isGuardianExists(int guardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qWarning() << "\033[33m Database is not open\033[0m";
        return false;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare("SELECT 1 FROM guardians WHERE guardian_id = ? LIMIT 1");
    query.addBindValue(guardianId);

    if(!query.exec())
    {
        qWarning() << "\033[33m Failed to check guardian existence:\033[0m" << query.lastError().text();
        return false;
    }

    return query.next();
}











































