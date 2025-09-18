#include "EnrollmentDAL.h"
#include "DatabaseManager/databaseManager.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

std::optional<int> DataAccess::EnrollmentDAL::createEnrollment(int personId, int schoolId, const QString &role, const QDate &start_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    QSqlQuery query(db);

    query.prepare(R"(
                    INSERT INTO enrollment(person_id, school_id, role, start_date, status)
                    VALUES(:personId, :schoolId, :role, startDate, 'Active') RETURNING enrollment_id
    )");

    query.bindValue(":personId", personId);
    query.bindValue(":schoolId", schoolId);
    query.bindValue(":role", role);
    query.bindValue(":startDate", start_date);


    if(query.exec() && query.next())
        return query.value(0).toInt();


    qWarning()<< "CREATE ENROLLMENT ERROR:" << query.lastError().text();
    return std::nullopt; // error
}


std::optional<DataModel::Enrollment> DataAccess::EnrollmentDAL::getEnrollment(int enrolment_id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(SELECT * FROM enrollment WHERE enrolmment_id = :id LIMIT 1)");
    query.bindValue(":id", enrolment_id);

    if(!query.exec())
    {
        qWarning() << "CREATE ENROLLMENT ERROR:"<< query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::Enrollment e;
        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id = query.value("person_id").toInt();
        e.school_id = query.value("school_id").toInt();
        e.role = DataModel::stringToRole(query.value("role").toString());
        e.start_date = query.value("start_date").toDate();
        e.end_date = query.value("end_date").toDate();
        e.status = DataModel::stringToStatus(query.value("status").toString());
        return e;
    }

    return std::nullopt;
}

bool DataAccess::EnrollmentDAL::updateEnrollment(int enrollment_id, const QString &role, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    UPDATE enrollment SET role = :role, end_date = :end,
                    status = CASE
                             WHEN :end IS NOT NULL AND :end < CURRENT_DATE
                             THEN 'Inactive'
                             ELSE status
                             END
                    WHERE enrollment_id = :id
)");

    query.bindValue(":role", role);
    query.bindValue(":end", end_date);
    query.bindValue(":id", enrollment_id);

    if(!query.exec())
    {
        qWarning() << "UPDATE ENROLLMENT ERORR:" <<query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool DataAccess::EnrollmentDAL::deleteEnrollment(int enrollment_id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("DELETE enrollments WHERE enrollment_id = :id");

    query.bindValue(":id", enrollment_id);

    if(!query.exec())
    {
        qWarning() << "DELETE ENROLLMENT ERORR:" <<query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;

}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getEnrollmentByPersonId(int personId)
{
    QVector<DataModel::Enrollment> enrollments;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                SELECT * FEROM enrollment WHERE person_id = :id
                ORDER BY start_date DESC)");

    query.bindValue(":id",personId);

    if(!query.exec())
    {
        qWarning() <<"GET ENROLLMENTS BY PERSON ID ERORR:"<< query.lastError().text();
        return enrollments;
    }

    while(query.next())
    {
        DataModel::Enrollment e;
        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id = query.value("person_id").toInt();
        e.school_id = query.value("school_id").toInt();
        e.role = DataModel::stringToRole( query.value("role").toString());
        e.start_date = query.value("start_date").toDate();
        e.end_date = query.value("end_date").toDate();
        e.status = DataModel::stringToStatus( query.value("status").toString());

        enrollments.append(e);
    }
    return enrollments;
}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getEnrollmentBySchoolId(int schoolId, const QString &role)
{
    QVector<DataModel::Enrollment> enrollments;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    QString sql= "SELECT * FROM enrollment WHERE school_id = :id";
    if(!role.isEmpty())
        sql += " AND role = :role";

    sql += "ORDER BY start_date DESC";

    query.prepare(sql);
    query.bindValue(":schoolId", schoolId);

    if(!role.isEmpty())
        query.bindValue(":role", role);

    if(!query.exec())
    {
        qWarning() << "GET ENROLLMENTS BY SCHOOL ID ERROR:" << query.lastError().text();
        return enrollments;
    }

    while(query.next())
    {
        DataModel::Enrollment e;

        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id = query.value("person_id").toInt();
        e.school_id = query.value("school_id").toInt();
        e.role = DataModel::stringToRole( query.value("role").toString());
        e.start_date = query.value("start_date").toDate();
        e.end_date = query.value("end_date").toDate();
        e.status = DataModel::stringToStatus( query.value("status").toString());

        enrollments.append(e);
    }
    return enrollments;
}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getActiveEnrollment(int schoolId)
{
    QVector<DataModel::Enrollment> enrollments;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    //get all active enrollment, except if end date more than current date
    query.prepare(R"(
        SELECT enrollment_id, person_id, school_id, role, start_date, end_date, status
        FROM enrollment
        WHERE school_id = :schoolId
          AND status = 'Active'
          AND (end_date IS NULL OR end_date >= CURRENT_DATE)
        ORDER BY start_date DESC
    )");

    query.bindValue(":id",schoolId);

    if(!query.exec())
    {
        qWarning() << "GET ACTIVE ENROLLMENTS ERROR:" << query.lastError().text();
        return enrollments;
    }

    while(query.next())
    {
        DataModel::Enrollment e;
        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id = query.value("person_id").toInt();
        e.school_id = query.value("school_id").toInt();
        e.role = DataModel::stringToRole( query.value("role").toString());
        e.start_date = query.value("start_date").toDate();
        e.end_date = query.value("end_date").toDate();
        e.status = DataModel::stringToStatus( query.value("status").toString());

        enrollments.append(e);
    }
    return enrollments;
}

std::optional<QVector<DataModel::Enrollment> > DataAccess::EnrollmentDAL::getPersonEnrollment(int personId, int schoolId)
{
    QVector<DataModel::Enrollment> enrollments;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(SELECT * FROM enrollment
                    WHERE person_id = :personId AND school_id = :schoolId
                    ORDER BY start_date DESC )");

    query.bindValue(":personId",personId);
    query.bindValue("schoolId",schoolId);

    if(!query.exec())
    {
        qWarning() << "GET PERSON ENROLLMENT ERORR:" << query.lastError().text();
        return std::nullopt;
    }

    while(query.next())
    {
        DataModel::Enrollment e;
        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id     = query.value("person_id").toInt();
        e.school_id     = query.value("school_id").toInt();
        e.role          = DataModel::stringToRole( query.value("role").toString());
        e.start_date    = query.value("start_date").toDate();
        e.end_date      = query.value("end_date").toDate();
        e.status        = DataModel::stringToStatus( query.value("status").toString());
        enrollments.append(e);
    }
    if(enrollments.isEmpty())
        return std::nullopt;
    return enrollments;
}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getAllEnrollment(int limit, int offset)
{
    QVector<DataModel::Enrollment> enrollments;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    query.prepare(R"(
        SELECT * FROM enrollment OFFSET :offset  LIMIT :limit
    )");
    query.bindValue(":limit", limit);
    query.bindValue(":offset",offset);


    if(!query.exec())
    {
        qWarning() << "GET ALL ENROLLMENTS ERROR:" << query.lastError().text();
        return enrollments;
    }

    while(query.next())
    {
        DataModel::Enrollment e;
        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id = query.value("person_id").toInt();
        e.school_id = query.value("school_id").toInt();
        e.role = DataModel::stringToRole( query.value("role").toString());
        e.start_date = query.value("start_date").toDate();
        e.end_date = query.value("end_date").toDate();
        e.status = DataModel::stringToStatus( query.value("status").toString());

        enrollments.append(e);
    }
    return enrollments;
}

bool DataAccess::EnrollmentDAL::isPersonEnrolled(int personId, int schoolId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT 1 FROM enrollment WHERE person_id = :personId
                    AND school_id = :schoolId
                    AND status = 'Active'
                    LIMIT 1 )");

    query.bindValue(":personId",personId);
    query.bindValue(":schoolId",schoolId);
    if(!query.exec())
    {
        qWarning() << "IS PERSON ENROLLED ERROR:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;

}

bool DataAccess::EnrollmentDAL::hasActiveEnrollmet(int personId, int schoolId, const QDate &end_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT 1 FROM enrollment
                    WHERE person_id = :personId
                    AND school_id = :schoolId
                    AND status = 'Active'
                    AND (end_date IS NULL OR end_date >= :end)
                    LIMIT 1
)");

    query.bindValue(":personId",personId);
    query.bindValue(":schoolId", schoolId);
    query.bindValue(":end", end_date);

    if(!query.exec())
    {
        qWarning() << "HAS ACTIVE ENROLLMENT ERROR:" << query.lastError().text();
        return false;
    }

    return query.next(); // if has row will return true
}



int DataAccess::EnrollmentDAL::getActiveStudentsCount(int enrollment_id)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    query.prepare(R"(
        SELECT COUNT(*)
        FROM enrollment e
        JOIN people p ON e.preson_id = p.person_id
        WHENE e.role = 'Student'
            AND e.status = 'Active'
            AND e.enrollment_id = :enrollment_id

)");

    query.bindValue(":enrollment_id", enrollment_id);

    if(!query.exec())
    {
        qWarning() << "GET ACTIVE STUDENTS COUNT ERROR:" << query.lastError().text();
        return 0;
    }
    if(query.next())
        return query.value(0).toInt();

    return 0;
}



int DataAccess::EnrollmentDAL::getActiveTeachersCount(int schoolId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
            SELECT COUNT(*)
            FROM enrollment e
            WHERE e.school_id = :schoolId
              AND e.role = 'Teacher'
              AND e.status = 'Active'
)");

    query.bindValue(":schoolId", schoolId);

    if(!query.exec())
    {
        qWarning() << "GET ACTIVE TEACHER COUNT ERORR:" << query.lastError().text();
        return 0;
    }

    if(query.next())
    {
        return query.value(0).toInt();
    }

    return 0;
}

int DataAccess::EnrollmentDAL::getActiveStaffCount(int schoolId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT COUNT(*)
        FROM enrollment
        WHERE school_id = :schoolId
          AND role = 'Staff'
          AND status = 'Active')");

    query.bindValue(":schoolId", schoolId);
    if(!query.exec())
    {
        qWarning() << "GET ACTIVE STAFF COUNT ERORR:" << query.lastError().text();
        return 0;
    }

    return query.next() ? query.value(0).toInt() : 0;
}

QMap<QString, int> DataAccess::EnrollmentDAL::getRoleDistribution(int schoolId)
{
    QMap<QString, int> distribution;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);


    query.prepare(R"(
                    SELECT  role, COUNT(*) AS count
                    FROM enrollment
                        WHERE school_id = :schoolId
                            AND status = 'Active'
                    GROUP BY role
)");


    query.bindValue(":schoolId", schoolId);

    if(!query.exec())
    {
        qWarning() <<"GET ROLE DISTRIBUTION ERROR:" << query.lastError().text();
        return distribution;
    }
    while (query.next())
    {
        distribution[query.value("role").toString()] = query.value("count").toInt();
    }

    return distribution;
}

bool DataAccess::EnrollmentDAL::transferPersonToSchool(int personId, int fromSchoolId, int toSchoolId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.transaction())
    {
        qWarning() <<"TRANSFER PERSON ERORR: Failed to start transaction" ;
        return false;
    }

    QSqlQuery query(db);

    query.prepare(R"(
                    UPDATE enrollment
                    SET school_id = :toSchoolId
                    WHERE person_id = :personId
                    AND school_id = :fromSchoolId
                    AND status = 'Active'
)");

    query.bindValue(":toSchoolId", toSchoolId);
    query.bindValue(":personId", personId);
    query.bindValue(":fromSchoolId", fromSchoolId);

    if(!query.exec())
    {
        qWarning() <<"TRANSFER PEROSN ERROR:" << query.lastError().text();
        db.rollback();
        return false;
    }
    if(!db.commit())
    {
        qWarning() << "TRANSFER PERSON ERROR: Failed to commit transaction";
        db.rollback();
        return false;
    }

    return true;

}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getEnrollmentHistory(int personId)
{
    QVector<DataModel::Enrollment> enrollments;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT e.enrollment_id, e.person_id, e.school_id, e.role, e.start_date, e.end_date, e.status
                        n.name AS school_name
                    FROM enrollment e
                    JOIN schools s ON e.school_id = s.school_id
                    WHERE e.person_id = :personId
                    ORDER BY e.start_date ASC
)");


query.bindValue(":personId", personId);

    if(!query.exec())
    {
    qWarning() << "GET ENROLLMENT HISTORY ERROR:" << query.lastError().text();
        return enrollments;
    }
    while(query.next())
    {
        DataModel::Enrollment e;

        e.enrollment_id = query.value("enrollment_id").toInt();
        e.person_id     = query.value("person_id").toInt();
        e.school_id     = query.value("school_id").toInt();
        e.role          = DataModel::stringToRole( query.value("role").toString());
        e.start_date    = query.value("start_date").toDate();
        e.end_date      = query.value("end_date").toDate();
        e.status        = DataModel::stringToStatus(query.value("status").toString());
        e.school_name   = query.value("school_name").toString();
        enrollments.append(e);
    }
    return enrollments;
}

QVector<DataModel::Enrollment> DataAccess::EnrollmentDAL::getRecentEnrollments(int schoolId, int days)
{
    QVector<DataModel::Enrollment> enrollments;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                SELECT enrollment_id, person_id, school_id, role, start_date, end_date, stauts
                FORM enrollment
                WHERE school_id = :schoolId
                AND start_date >= (CURRENT_DATE - (:days || 'days')::INTERVAL)
                ORDER BY start_date DESC
    )");

    query.bindValue(":schoolId",schoolId);
    query.bindValue(":days", days);

    if(!query.exec())
    {
        qWarning() << "GET RECENT ENROLLMENT ERROR:" << query.lastError().text();
        return enrollments;
    }

    while(query.next())
    {

            DataModel::Enrollment e;

            e.enrollment_id = query.value("enrollment_id").toInt();
            e.person_id     = query.value("person_id").toInt();
            e.school_id     = query.value("school_id").toInt();
            e.role          = DataModel::stringToRole(query.value("role").toString());
            e.start_date    = query.value("start_date").toDate();
            e.end_date      = query.value("end_date").toDate();
            e.status        = DataModel::stringToStatus(query.value("status").toString());
            enrollments.append(e);

    }

    return enrollments;
}

int DataAccess::EnrollmentDAL::getTotalCount()
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT COUNT(*) FROM enrollment");

    if(!query.exec())
    {
        qWarning() << "GET TOTAL ENROLLMENT COUNT ERROR:" << query.lastError().text();
        return 0;
    }
    if(query.next())
        return query.value(0).toInt();

    return 0;
}























































