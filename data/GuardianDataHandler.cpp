#include "GuardianDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"
#include"data/PeopleDataHandler.h"


#include<QSqlQuery>
#include<QSqlDatabase>
#include<QSqlError>

#include <QStringBuilder>


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
                        SELECT sg.student_person_id , sg.guardian_id, sg.relationship, sg.is_primary,
                                s.first_name || ' ' || s.second_name as studnet_name,
                                g.first_name || ' ' || g.second_name as guardian_name

                        FROM student_guardian sg
                        JOIN people s ON s.perosn_id = sg.student_person_id
                        JOIN guardian gr ON gr.guardian_id = sg.guardian_id
                        JOIN people g ON g.person_id = gr.person_id
                        WHERE sg.student_guardian_id = ?

)");

    infoQuery.addBindValue(studentGuardianId);

    int studentPersonId = -1;
    int guardianId = -1;
    QString relationship;
    bool wasPrimary = false;
    QString studentName, guardianName;

    if(infoQuery.exec() && infoQuery.next())
    {
        studentPersonId = infoQuery.value("student_person_id").toInt();
        guardianId = infoQuery.value("guardian_id").toInt();
        relationship = infoQuery.value("relationship").toString();
        wasPrimary = infoQuery.value("is_primary").toBool();
        studentName = infoQuery.value("student_name").toString();
        guardianName = infoQuery.value("guardian_name").toString();
    }

    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM student_guardian WHERE student_guardian_id = ? ");
    deleteQuery.addBindValue(studentGuardianId);

    if(!deleteQuery.exec())
    {
        qCritical() <<"\033[31m Failed to remove student-guardian relationship " << deleteQuery.lastError().text();
        Logger::instance().error("Failed to remove student-guardian relationship " + deleteQuery.lastError().text());
        db.rollback();
        return false;
    }


    if(deleteQuery.numRowsAffected() == 0)
    {
        qWarning() <<"\033[33m No rows affected when removing relationship\033[0m";
        Logger::instance().warning("No rows affected when removing relationship");
        db.rollback();
        return false;
    }

    if(wasPrimary && studentGuardianId  != 1)
    {
        QSqlQuery findNewPrimaryQuery(db);
        findNewPrimaryQuery.prepare(R"(

                SELECT student_guardian_id
                FROM student_guardians
                WHERE student_person_id = ?
                ORDER BY student_guardian_id ASC
                LIMIT 1
)");
        findNewPrimaryQuery.addBindValue(studentPersonId);

        if(findNewPrimaryQuery.exec() && findNewPrimaryQuery.next())
        {
            int newPrimaryId = findNewPrimaryQuery.value("student_guardian_id").toInt();

            QSqlQuery updatePrimaryQuery(db);
            updatePrimaryQuery.prepare("UPDATE student_guardians SET is_priamry = true WHERE student_guardian_id = ?");
            updatePrimaryQuery.addBindValue(newPrimaryId);

            if(updatePrimaryQuery.exec())
            {
                qInfo() <<"\033[33m Auto-assigned new primary guardian for student: \033[0m" << studentName;
            }
        }
    }

    if(!db.commit())
    {
        qCritical() <<"\033[31m Failed to commit transaction. \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to commit transaction. " + db.lastError().text());
        db.rollback();
        return false;
    }

    qInfo() <<"\033[32m Successfully removed student-guardian relationship ID: " << studentGuardianId
            <<" Student: " << studentName
            <<" Guardian: " << guardianName
            <<" Relationship: " <<relationship
            <<" Was Primary: " <<(wasPrimary ? "Yes" : "No") ;

    return true;
}

bool DataAccess::GuardianDataHandler::updateStudentGuardianRelationship(int studentGuardianId, const QString &relationship, bool isPrimary)
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
            SELECT student_guardian_id, studnet_person_id, relationship, is_primary
            FROM student_guardians
            WHERE student_guardian_id = ?
)")    ;


    checkQuery.addBindValue(studentGuardianId);

    if(!checkQuery.exec() || !checkQuery.next())
    {
        qCritical() << "\033[31m Student-guardian relationship not found with ID:\033[0m" << studentGuardianId;
        Logger::instance().error("Student-guardian relationship not found with ID: " + QString::number(studentGuardianId));
        db.rollback();
        return false;
    }

    int studentPersonId = checkQuery.value("student_person_id").toInt();
    int guardianId = checkQuery.value("guardian_id").toInt();
    QString currentrelationship = checkQuery.value("relationship").toString();
    bool currentIsPrimary = checkQuery.value("is_primary").toBool();

    // if the cuurent is primary was chenged, I will chenge the is primary in database
    if(isPrimary && !currentIsPrimary)
    {
        QSqlQuery updatePrimaryQuery(db);
        updatePrimaryQuery.prepare(R"(
                        UPDATE student_guardians SET is_priamry = false
                        WHERE student_person_id = ? AND student_guardian_id != ?
    )");
         updatePrimaryQuery.addBindValue(studentPersonId);
         updatePrimaryQuery.addBindValue(studentGuardianId);

         if(!updatePrimaryQuery.exec())
         {
             qWarning() << "\033[33m Warning: Failed to update existing primary relationships for student ID:\033[0m" << studentPersonId;
             Logger::instance().warning("Warning: Failed to update existing primary relationships for student ID: " + QString::number(studentPersonId));
             // don't return false
         }


         // warning, there are no alternative primary guardain
         if(!isPrimary && currentIsPrimary)
         {
             QSqlQuery checkOtehrPrimaryQuery(db);
             checkOtehrPrimaryQuery.prepare(R"(
                SELECT COUNT(*) FROM student_guardians
                WHERE student_person_id = ? AND is_primary = true
                      AND student_guardian_id != ?

)");

             checkOtehrPrimaryQuery.addBindValue(studentPersonId);
             checkOtehrPrimaryQuery.addBindValue(studentGuardianId);

             int otherPrimaryCount = 0;
             if(checkOtehrPrimaryQuery.exec() && checkOtehrPrimaryQuery.next())
             {
                 otherPrimaryCount = checkOtehrPrimaryQuery.value(0).toInt();
             }

             if(otherPrimaryCount == 0)
             {
                 qWarning() << "\033[33m Removing primary status from the only guardian for student ID: \033[0m"<< studentPersonId;
                 Logger::instance().warning("Removing primary status from the only guardian for student ID: " + QString::number(studentPersonId));

             }
         }
    }

         QSqlQuery updateQuery(db);
         updateQuery.prepare(R"(
                        UPDATE student_gaurdians
                        SET relationship = ?
                            is_primary = ?
                        WHERE studen_guardian_id = ?
)");
         updateQuery.addBindValue(relationship);
         updateQuery.addBindValue(isPrimary);
         updateQuery.addBindValue(studentGuardianId);


         if(!updateQuery.exec())
         {
             qCritical() << "\033[31m Failed to update student-guardian relationship ID:\033[0m" << studentGuardianId
                         << "Error:" << updateQuery.lastError().text();
             Logger::instance().error("Failed to update student-guardian relationship ID: " + QString::number(studentGuardianId) +
                                      " Error: " + updateQuery.lastError().text());
             db.rollback();
             return false;
         }

         if(updateQuery.numRowsAffected() == 0)
         {
             qWarning() << "\033[33m No rows affected when updating relationship ID:\033[0m" << studentGuardianId;
             Logger::instance().warning("No rows affected when updating relationship ID: " + QString::number(studentGuardianId));
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

         qInfo() << "\033[32m Successfully updated student-guardian relationship ID:\033[0m" << studentGuardianId
                 << "Student ID:" << studentPersonId
                 << "Guardian ID:" << guardianId
                 << "Relationship:" << currentrelationship << "→" << relationship
                 << "Primary:" << (currentIsPrimary ? "Yes" : "No") << "→" << (isPrimary ? "Yes" : "No");

         return true;

}

bool DataAccess::GuardianDataHandler::setPrimaryGuardian(int studentPersonId, int guardianId)
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

    // check relationship between student and guardain
    QSqlQuery checkRelationshipQuery(db);
    checkRelationshipQuery.prepare("SELECT 1 FROM student_guardians WHERE studen_person_id = ? AND guardian_id = ? LIMIT 1");

    checkRelationshipQuery.addBindValue(studentPersonId);
    checkRelationshipQuery.addBindValue(guardianId);

    if(!checkRelationshipQuery.exec() || !checkRelationshipQuery.next())
    {
        qCritical() << "\033[31m Relationship not found between student ID:\033[0m" << studentPersonId
                    << "and guardian ID:" << guardianId;
        Logger::instance().error("Relationship not found between student ID: " + QString::number(studentPersonId) +
                                 " and guardian ID: " + QString::number(guardianId));
        db.rollback();
        return false;
    }




// remove all student's primaty relationship

    QSqlQuery removePrimaryQuery(db);
    removePrimaryQuery.prepare(R"(
                        UPDATE student_guardains
                        SET is_primary = false
                        WHERE student_person_id = ?
)")    ;
    removePrimaryQuery.addBindValue(studentPersonId);

    if(!removePrimaryQuery.exec())
    {
        qCritical() << "\033[31m Failed to remove primary status from other guardian for student ID:\033[0m" << studentPersonId;
        Logger::instance().error("Failed to remove primary status from other guardian for student ");
        db.rollback();
        return false;
    }




    // make the guardain primary

    QSqlQuery setPrimaryQuery(db);
    setPrimaryQuery.prepare(R"(
                    UPDATE student_guardains
                    SET is_primary = true
                    WHERE student_person_id = ?
                    AND guardian_id = ?

)");


    setPrimaryQuery.addBindValue(studentPersonId);
    setPrimaryQuery.addBindValue(guardianId);

    if(!setPrimaryQuery.exec())
    {
        qCritical() << "\033[31m Failed to set guardian as primary for student ID:\033[0m" << studentPersonId;
        Logger::instance().error("Failed to set guardian as primary for student");
        db.rollback();
        return false;
    }



    if(setPrimaryQuery.numRowsAffected() == 0 )
    {
        qWarning() << "\033[33m No rows affected when setting primary guardian for student ID:\033[0m" << studentPersonId;
        Logger::instance().warning("No rows affected when setting primary guardian for student");
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

    qInfo() << "\033[32m Successfully set primary guardian for student ID:\033[0m" << studentPersonId
            << "Guardian ID:" << guardianId;


    return true;

}






QVector<DataModel::StudentGuardian> DataAccess::GuardianDataHandler::getStudentGuardians(int studentPersonId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db  = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return QVector<DataModel::StudentGuardian>();
    }



    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
    SELECT
        sg.student_guardian_id,
        sg.student_person_id,
        sg.guardian_id,
        sg.relationship,
        sg.is_priamry,

        -- student info

        s.first_name AS s_first_name,
        s.second_name AS s_second_name,
        s.third_name AS s_third_name,
        s.fourth_name AS s_fourth_name,

        -- gaurdain info

        g.guardian_id ,
        g.occupation ,
        g.phone_number,
        g.education_level,

        p.first_name AS g_first_name,
        p.second_name AS g_second_name,
        p.third_name AS g_third_name,
        p.fourth_name AS g_fourth_name,
        p.gender AS g_gender,
        p.date_of_birth AS g_dob

    FROM student_guardians sg
    JOIN people s ON s.person_id = sg.student_person_id
    JOIN guardians g ON g.guardian_id = sg.guardian_id
    JOIN people p ON p.person_id = g.person_id
    WHERE sg.student_person_id = ?
    ORDER BY sg.is_primary DESC

)");

    query.addBindValue(studentPersonId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get student guardians for student ID:\033[0m" << studentPersonId
                    << "Error:" << query.lastError().text();
        Logger::instance().error("Failed to get student guardians for student ID: " + QString::number(studentPersonId) +
                                 " Error: " + query.lastError().text());
        return QVector<DataModel::StudentGuardian>();
    }

    QVector<DataModel::StudentGuardian> studentGuardiansList;

    int count = 0;
    while(query.next())
    {
        DataModel::StudentGuardian sg;

        sg.student_guardian_id = query.value("student_guardian_id").toInt();
        sg.student_person_id = query.value("student_person_id").toInt();
        sg.guardian_id = query.value("guardian_id").toInt();
        sg.relationship = query.value("relationship").toString();
        sg.is_primary  = query.value("is_primary").toBool();

        //guardian
        sg.guardianInfo.guardian_id = query.value("guardian_id").toInt();
        sg.guardianInfo.person_id = query.value("guardian_id").toInt(); // temprory
        sg.guardianInfo.occupation = query.value("occupation").toString();
        sg.guardianInfo.work_phone = query.value("phone_number").toString();
        sg.guardianInfo.education_level = query.value("education_level").toString();
        sg.guardianInfo.first_name = query.value("g_first_name").toString();
        sg.guardianInfo.second_name = query.value("g_second_name").toString();
        sg.guardianInfo.third_name = query.value("g_third_name").toString();
        sg.guardianInfo.fourth_name = query.value("g_fourth_name").toString();
        sg.guardianInfo.full_name = sg.guardianInfo.first_name + " " +
                                    sg.guardianInfo.second_name + " " +
                                    sg.guardianInfo.third_name + " " +
                                    sg.guardianInfo.fourth_name;

        sg.guardianInfo.gender = query.value("g_gender").toChar();
        sg.guardianInfo.date_of_birth = query.value("g_dob").toDate();

        studentGuardiansList.append(sg);
        count++;

    }
        qDebug() << "\033[32m Retrieved" << count << "guardians for student ID:\033[0m" << studentPersonId;
    return studentGuardiansList;

}

DataModel::Guardian DataAccess::GuardianDataHandler::getPrimaryGuardianByStudent(int studentPersonId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return DataModel::Guardian();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare(R"(
       SELECT
            g.guardian_id,
            g.person_id,
            g.occuption,
            g.phone_number,
            g.education_level,

            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            p.date_of_birth,
            sg.relationship
        FROM student_guardians sg
        JOIN guardians g ON g.guardian_id = sg.guardian_id
        JOIN people p ON p.person_id = g.person_id
        WHERE sg.student_person_id = ? AND sg.is_primary = true
        LIMIT 1
)");

    query.addBindValue(studentPersonId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get primary guardian for student ID:\033[0m" << studentPersonId
                    << "Error:" << query.lastError().text();
        Logger::instance().error("Failed to get primary guardian for student ID: " + QString::number(studentPersonId) +
                                 " Error: " + query.lastError().text());
        return DataModel::Guardian();
    }
    DataModel::Guardian g;
    if(query.next())
    {
        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();

        g.full_name = g.first_name + " " +
                      g.second_name + " " +
                      g.third_name + " " +
                      g.fourth_name;
        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();
        g.relationship = query.value("relationship").toString();
        qDebug() << "\033[32m Retrieved primary guardian for student ID:\033[0m" << studentPersonId
                 << "Name:" << g.full_name;
    }
    else
    {
        qWarning() << "\033[33m No primary guardian found for student ID:\033[0m" << studentPersonId;
    }

    return g;
}

QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::searchGuardianByPhoneNumber(const QString &phoneNumber)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return QVector<DataModel::Guardian>();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare(R"(
             SELECT
                g.guardian_id,
                g.person_id,
                g.occupation,
                g.phone_number,
                g.education_number,

                p.first_name,
                p.second_name,
                p.third_name,
                p.fourth_name,
                p.gender,
                p.date_of_birth
            FROM guardians g
            JOIN people p ON p.person_id = g.person_id
            WHERE g.phone_number LIKE ?
            ORDER BY p.first_name, p.second_name
)");

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to search guardians by phone number:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to search guardians by phone number: " + query.lastError().text());
        return QVector<DataModel::Guardian>();
    }
    QVector<DataModel::Guardian> guardiansList;
    int count = 0;
    while(query.next())
    {
        DataModel::Guardian g;
        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();
        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();

        guardiansList.append(g);
        count++;
    }

        qDebug() << "\033[32m Found" << count << "guardians with phone number containing:\033[0m" << phoneNumber;

    return guardiansList;
}

QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::searchGuardianByName(const QString &fname, const QString &sname, const QString &tname)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return QVector<DataModel::Guardian>();
    }

    QString whereClause;
    QVector<QString> bindValues;

    auto addCondition = [&](const QString& field, const QString& value)
    {
        if(!value.isEmpty())
        {
            if(!whereClause.isEmpty()) whereClause += " AND ";

            whereClause += field % " LIKE ? ";
            bindValues << (value.trimmed() + "%");
        }
    };

    addCondition("p.first_name", fname);
    addCondition("p.second_name", sname);
    addCondition("p.third_name", tname);

    if(whereClause.isEmpty()) return QVector<DataModel::Guardian>();

    QString sql = QString(R"(
                    SELECT
                        guardian_id,
                        person_id,
                        occupation,
                        phone_number,
                        education_level,

                        first_name,
                        second_name,
                        third_name,
                        fourth_name,
                        gender,
                        date_of_birth
                    FROM guardians g
                    JOIN people p USING(person_id)
                    WHERE %1
                    ORDER BY first_name, second_name

)").arg(whereClause);


    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(sql);

    for(const QString& value : bindValues)
    {
        query.addBindValue(value);
    }


    QVector<DataModel::Guardian> guardiansList;

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to search guardians by name:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to search guardians by name: " + query.lastError().text());
        return QVector<DataModel::Guardian>();
    }

    while(query.next())
    {
        DataModel::Guardian g;

        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();

        QStringList parts = {g.first_name, g.second_name, g.third_name, g.fourth_name};
        parts.removeAll("");
        g.full_name = parts.join(" ");

        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();

        guardiansList.append(g);
    }

    return guardiansList;
}

QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::searchGuardianByOccupation(const QString &occupation)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return QVector<DataModel::Guardian>();
    }

    if(occupation.trimmed().isEmpty())
    {
        qWarning() <<"\033[33m Occupation search criteria is empty! \033[0m";
        return QVector<DataModel::Guardian>();
    }


    QString whereClause;
    QVector<QString> bindValues;

    auto addCondition = [&](const QString& field, const QString& value)
    {
        if(!value.isEmpty())
        {
            if(!whereClause.isEmpty()) whereClause += " AND ";

            whereClause += field % " LIKE ? ";
            bindValues << (value.trimmed() + "%");
        }
    };


    addCondition("g.occupation", occupation);



    QString sql = QString(R"(
                SELECT
                    g.guardian_id,
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
                JOIN people p USING(person_id)
                WHERE %1
                ORDER BY p.first_name, p.second_name
                LIMIT 100
)").arg(whereClause);

    QSqlQuery query(db);
    query.setForwardOnly(true);
    if(!query.prepare(sql))
    {
        qCritical() << "\033[31m Failed to prepare query:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to prepare query: " + query.lastError().text());
        return     QVector<DataModel::Guardian> ();
    }


    for(const QString& value : bindValues)
        query.addBindValue(value);

    QVector<DataModel::Guardian> guardiansList;

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to search guardians by occupation:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to search guardians by occupation " + query.lastError().text());
         return QVector<DataModel::Guardian>();
    }

    while(query.next())
    {
        DataModel::Guardian g;
        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();
        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();
        QStringList parts = {g.first_name, g.second_name, g.third_name, g.fourth_name};
        parts.removeAll("");

        g.full_name = parts.join(" ");

        guardiansList.append(g);
    }

        qDebug() << "\033[32m Found" << guardiansList.size() << "guardians with occupation:\033[0m" << occupation;

    return guardiansList;

}

QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::searchGuardiansByEducationLevel(const QString &educationLevel)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return      QVector<DataModel::Guardian> ();
    }



    if(educationLevel.trimmed().isEmpty())
    {
        qWarning() << "\033[33m Education level search criteria is empty\033[0m";
        return QVector<DataModel::Guardian> ();
    }


    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
                    SELECT
                        g.guardian_id,
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
                    JOIN people p USING(person_id)
                    WHERE g.education_level LIKE ?
                    ORDER BY p.first_name, p.second_name
)");

    QString searchPattern = educationLevel.trimmed() + "%";
    query.addBindValue(searchPattern);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to search guardians by education level:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to search guardians by education level: " + query.lastError().text());
        return QVector<DataModel::Guardian> ();
    }
    QVector<DataModel::Guardian> guardiansList;
    while(query.next())
    {
        DataModel::Guardian g;
        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();
        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();
        QStringList parts = {g.first_name, g.second_name, g.third_name, g.fourth_name};
        parts.removeAll("");

        g.full_name = parts.join(" ");

        guardiansList.append(g);
    }

    qDebug() << "\033[32m Found" << guardiansList.size() << "guardians with education level:\033[0m" << educationLevel;
    return guardiansList;
}
QVector<DataModel::Guardian> DataAccess::GuardianDataHandler::searchGuardiansByStudentName(
    const QString &f_studentName,
    const QString &s_studentName,
    const QString &t_studentName,
    const QString &ft_studentName)
{
    QVector<DataModel::Guardian> guardianList;

    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return guardianList;
    }

    QString whereClause;
    QVector<QString> bindValues;

    auto addCondition = [&](const QString& field, const QString& value)
    {
        if (!value.trimmed().isEmpty())
        {
            if (!whereClause.isEmpty()) whereClause += " AND ";
            whereClause += QString("LOWER(%1) LIKE LOWER(?)").arg(field);
            bindValues << (value.trimmed() + "%");
        }
    };

    addCondition("s.first_name", f_studentName);
    addCondition("s.second_name", s_studentName);
    addCondition("s.third_name", t_studentName);
    addCondition("s.fourth_name", ft_studentName);

    if (whereClause.isEmpty())
    {
        qWarning() << "\033[33m No Student name criteria provided \033[0m";
        return guardianList;
    }

    QString sql = QString(R"(
        SELECT DISTINCT
            g.guardian_id,
            g.person_id,
            g.occupation,
            g.phone_number,
            g.education_level,

            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            p.date_of_birth,

            s.first_name AS s_first_name,
            s.second_name AS s_second_name,
            s.third_name AS s_third_name,
            s.fourth_name AS s_fourth_name,
            sg.relationship,
            sg.is_primary
        FROM guardians g
        JOIN people p USING(person_id)
        JOIN student_guardians sg ON sg.guardian_id = g.guardian_id
        JOIN people s ON s.person_id = sg.student_person_id
        WHERE %1
        ORDER BY s.first_name, s.second_name, p.first_name, p.second_name
        LIMIT 100
    )").arg(whereClause);

    QSqlQuery query(db);
    query.setForwardOnly(true);

    if (!query.prepare(sql))
    {
        qCritical() << "\033[31m Failed to prepare query:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to prepare query: " + query.lastError().text());
        return guardianList;
    }

    for (const QString& value : bindValues)
        query.addBindValue(value);

    if (!query.exec())
    {
        qCritical() << "\033[31m Failed to execute guardian search by student name:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to execute guardian search by student name: " + query.lastError().text());
        return guardianList;
    }

    while (query.next())
    {
        DataModel::Guardian g;

        g.guardian_id = query.value("guardian_id").toInt();
        g.person_id = query.value("person_id").toInt();
        g.occupation = query.value("occupation").toString();
        g.work_phone = query.value("phone_number").toString();
        g.education_level = query.value("education_level").toString();

        g.first_name = query.value("first_name").toString();
        g.second_name = query.value("second_name").toString();
        g.third_name = query.value("third_name").toString();
        g.fourth_name = query.value("fourth_name").toString();

        QStringList parts = { g.first_name, g.second_name, g.third_name, g.fourth_name };
        parts.removeAll("");
        g.full_name = parts.join(" ");

        g.gender = query.value("gender").toChar();
        g.date_of_birth = query.value("date_of_birth").toDate();

        g.studentFirstName = query.value("s_first_name").toString();
        g.studentSecondName = query.value("s_second_name").toString();
        g.studentThirdName = query.value("s_third_name").toString();
        g.studentFourthName = query.value("s_fourth_name").toString();

        QStringList nameParts = { g.studentFirstName, g.studentSecondName, g.studentThirdName, g.studentFourthName };
        nameParts.removeAll("");
        g.studentFullName = nameParts.join(" ");

        g.relationship = query.value("relationship").toString();
        g.isPrimary = query.value("is_primary").toBool();

        guardianList.append(g);
    }

    qDebug() << "\033[32m Found" << guardianList.size()
             << "guardians for students matching criteria.\033[0m";

    return guardianList;
}

std::optional<int> DataAccess::GuardianDataHandler::getGuardiansCount()
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return std::nullopt;
    }

    QSqlQuery query (db);
    query.setForwardOnly(true);
    query.prepare("SELECT COUNT(*) AS guardian_count FROM guardians");

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get guardian count:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to get guardian count: " + query.lastError().text());
        return std::nullopt;
    }

    if(query.next())
    {
        int count = query.value("guardian_count").toInt();
        qDebug() << "\033[32m Total guardians count:\033[0m" << count;
        return count;
    }

    qWarning() << "\033[0mSome thing wrong!\033[0m";
    return std::nullopt;
}

bool DataAccess::GuardianDataHandler::isGuardianExists(int guardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database is not open \033[0m"  << db.lastError().text();
        Logger::instance().error("Database is not open" + db.lastError().text());

        return false;
    }

    QSqlQuery query(db);

    query.setForwardOnly(true);
    query.prepare("SELECT 1 FROM guardians WHERE guardian_id = ? LIMIT 1");
    query.addBindValue(guardianId);

    if(!query.exec())
    {
        qCritical() << "\033[33m Failed to check guardian existence:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to check guardian existence " +query.lastError().text());
        return false;
    }

    return query.next();
}

bool DataAccess::GuardianDataHandler::isStudentGuardianRelationshipExists(int studentPersonId, int guardianId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT 1 FROM student_guardians WHERE student_person_id = ? AND guardian_id = ? LIMIT 1");

    query.addBindValue(studentPersonId);
    query.addBindValue(guardianId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to check student-guardian relationship:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to check student-guardian relationship: " + query.lastError().text());
        return false;
    }
    return query.next();
}

bool DataAccess::GuardianDataHandler::hasStudentAnyGuardian(int studentPersonId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare("SELECT 1 FROM student_guardians WHERE student_person_id = ? LIMIT 1");
    query.addBindValue(studentPersonId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to check student-guardian has any relationship:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to check student-guardian has any relationship: " + query.lastError().text());
        return false;
    }

    return query.next();
}

DataModel::Guardian DataAccess::GuardianDataHandler::getPrimaryGuardianInfo(int studentPersonId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return DataModel::Guardian ();
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare(R"(
        SELECT
            g.guardian_id,
            g.person_id,
            g.occupation,
            g.phone_number,
            g.education_level,

            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            p.date_of_birth,
            sg.relationshipl
        FROM student_guardians sg
        JOIN guardians g ON g.guardian_id = sg.guardian_id
        JOIN people p ON p.person_id = g.person_id
        WHERE sg.student_person_id = ? AND sg.is_primary = true
        LIMIT 1
)");

    query.addBindValue(studentPersonId);

    if(!query.exec())
    {
        qCritical() << "\033[31m Failed to get primary guardian info:\033[0m" << query.lastError().text();
        Logger::instance().error("Failed to get primary guardian info: " + query.lastError().text());
        return DataModel::Guardian();
    }
    DataModel::Guardian guardian;
    if(query.next())
    {
        guardian.guardian_id = query.value("guardian_id").toInt();
        guardian.person_id = query.value("person_id").toInt();
        guardian.occupation = query.value("occupation").toString();
        guardian.work_phone = query.value("phone_number").toString();
        guardian.education_level =query.value("education_level").toString();
        guardian.first_name = query.value("first_name").toString();
        guardian.second_name = query.value("second_name").toString();
        guardian.third_name = query.value("third_name").toString();
        guardian.fourth_name = query.value("fourth_name").toString();
        QStringList parts = {guardian.first_name,
                                 guardian.second_name,
                                     guardian.third_name,
                                         guardian.fourth_name};
        parts.removeAll("");
        guardian.full_name = parts.join(" ");

        guardian.gender = query.value("gender").toChar();
        guardian.date_of_birth = query.value("date_of_birth").toDate();
        guardian.relationship = query.value("relationship").toString();
    }

    return guardian;
}

bool DataAccess::GuardianDataHandler::hasPrimaryGuardian(int studentPersonId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open.\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open: " + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT 1 FROM student_guardians WHERE student_person_id = ? AND is_primary = true LIMIT 1");
    query.addBindValue(studentPersonId);

if(!query.exec())
{
    qCritical() << "\033[31m Failed to check primary guardian:\033[0m" << query.lastError().text();
    Logger::instance().error("Failed to check primary guardian" + query.lastError().text());
    return false;
}
    return query.next();
}

// bool DataAccess::GuardianDataHandler::updateGuardinOccupation(int guardianId, const QString &occupation)
// {
        // I will complete it tmrw  -- GN
// }
































































