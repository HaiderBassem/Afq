#include "StudentEnrollmentDAL.h"
#include "DatabaseManager/databaseManager.h"

#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>


std::optional<int> DataAccess::StudentEnrollmentDAL::enrollStudentInClass(int enrollmentId, int yearId, const QDate &start_date)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    INSERT INTO student_enrollment(enrollment_id, class_id, year_id, start_date, status)
                    VALUES(:enrollmentId,
                            (SELECT class_id FROM enrollment e
                            JOIN student_enrollment se ON e.enrollment_id = se.enrollment_id
                            WHERE e.enrollment_id = :enrollmentId LIMIT 1)
                            :yearId, :start, 'Active' ) RETURNING student_enrollment_id
                   )");

    query.bindValue(":enrollmentId", enrollmentId);
    query.bindValue(":yearId", yearId);
    query.bindValue(":start", start_date);

    if(query.exec() && query.next())
    {
        return query.value(0).toInt();
    }

    qWarning() << "ENROLL STUDENT IN CLASS ERROR:" <<query.lastError().text();
    return std::nullopt;
}

std::optional<DataModel::StudentEnrollment> DataAccess::StudentEnrollmentDAL::getStudentEnrollmentById(int studentEnrollmentId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM student_enrollment WHERE student_enrollment_id = :enrollmentId LIMIT 1");
    query.bindValue(":enrollmentId", studentEnrollmentId);

    if(!query.exec())
    {
        qWarning() << "GET STUDENT ENROLLMNET BY ID ERROR:" << query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::StudentEnrollment se;
        se.student_enrollment_id  = query.value("student_enrollment_id").toInt();
        se.enrollment_id = query.value("enrollment_id").toInt();
        se.class_id = query.value("class_id").toInt();
        se.year_id = query.value("year_id").toInt();
        se.start_date = query.value("start_date").toDate();
        se.end_date = query.value("end_date").toDate();
        se.status  = DataModel::stringToStatus(query.value("status").toString());
        return se;
    }
    return std::nullopt;
}

bool DataAccess::StudentEnrollmentDAL::updatestudentEnrollment(int studentEnrollmentId, int classId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE student_enrollment SET class_id = :classId, updated_at  = NOW(), WHERE student_enrollment_id = :id");
    query.bindValue(":id", studentEnrollmentId);
    query.bindValue(":classId", classId);

    if(!query.exec())
    {
        qWarning() << "UPDATE STUDENT ENROLLMENT ERROR:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool DataAccess::StudentEnrollmentDAL::deleteStudentEnrollment(int StudentEnrollmentId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("DELETE student_enrollment WHERE student_enrollment_id = :id");
    query.bindValue(":id", StudentEnrollmentId);

    if(!query.exec())
    {
        qWarning() << "DELETE STUDENT ENROLLMENT ERROR:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

QVector<DataModel::StudentEnrollment> DataAccess::StudentEnrollmentDAL::getStudentsByClass(int classId, int yearId)
{
    QVector<DataModel::StudentEnrollment> students;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT se.student_enrollment_id, se_enorllment_id, se.class_id, se.year_id,
                    se.start_date, se.end_date, se.status,
                    p.person_id, p.first_name, p.second_name, p.third_name, p.fourth_name
                    FROM student_enrollment se
                    JOIN enrollment e ON se.enrollmant_id = e.enrollment_id
                    JOIN people p ON e.person_id = p.person_id
                    WHERE se.class_id = :classId
                    AND se.year_id = :yaerId
)");

    query.bindValue(":classId",classId);
    query.bindValue(":yearId", yearId);

    if(!query.exec())
    {
        qWarning() <<"GET STUDENTS BY CLASS ERROR:" << query.lastError().text();
        return students;
    }

    while(query.next())
    {
        DataModel::StudentEnrollment s;

        s.student_enrollment_id = query.value("student_enrollment_id").toInt();
        s.enrollment_id = query.value("enrollment_id").toInt();
        s.class_id = query.value("class_id").toInt();
        s.year_id = query.value("year_id").toInt();
        s.start_date = query.value("start_date").toDate();
        s.end_date = query.value("end_date").toDate();
        s.status = DataModel::stringToStatus(query.value("status").toString());
        QString fullName = query.value("first_name").toString() +
                           query.value("second_name").toString() +
                            query.value("third_name").toString() +
                           query.value("fourth_name").toString();
        s.full_name = fullName.trimmed();

        students.append(s);
    }
    return students;

}




QVector<DataModel::StudentEnrollment> DataAccess::StudentEnrollmentDAL::getStudentsEnrollmentHistory(int enrollmentId)
{
    QVector<DataModel::StudentEnrollment> history;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT se.student_enrollment_id, se.enrollment_id, se.class_id, se.year_id,
               se.start_date, se.end_date, se.status,
               c.name AS class_name, c.section
        FROM student_enrollment se
        JOIN classes c ON se.class_id = c.class_id
        WHERE se.enrollment_id = :enrollmentId
        ORDER BY se.start_date ASC
    )");

    query.bindValue(":enrollmentId", enrollmentId);

    if (!query.exec()) {
        qWarning() << "GET STUDENT ENROLLMENT HISTORY ERROR:" << query.lastError().text();
        return history;
    }

    while (query.next()) {
        DataModel::StudentEnrollment se;
        se.student_enrollment_id = query.value("student_enrollment_id").toInt();
        se.enrollment_id         = query.value("enrollment_id").toInt();
        se.class_id              = query.value("class_id").toInt();
        se.year_id               = query.value("year_id").toInt();
        se.start_date            = query.value("start_date").toDate();
        se.end_date              = query.value("end_date").toDate();
        se.status                = DataModel::stringToStatus( query.value("status").toString());
        se.class_name            = query.value("class_name").toString();
        se.section               = query.value("section").toString();

        history.append(se);
    }

    return history;
}


std::optional<DataModel::StudentEnrollment> DataAccess::StudentEnrollmentDAL::getStudentCurrentClass(int enrollmentId, int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                SELECT se.student_enrollment_id, se.enrollment_id, se.class_id, se.year_id,
                    se.start_date, se.end_date, se.status,
                    c.name, AS class_name, c.section AS class_section
                FROM student_enrollment se
                JOIN classes c ON se.class_id = c.class_id
                WHERE se.enrollment_id = :enrollmentId
                AND se.year_id = :yearId
                AND (se.end_date IS NULL OR se.end_date >= CURRENT_DATE)
                LIMIT 1 )");

    query.bindValue("enrollmentId", enrollmentId);
    query.bindValue(":yearId",yearId);

    if(!query.exec())
    {
        qWarning() << "GET STUDENT CURRENT CLASS ERROR:" <<query.lastError().text();
        return std::nullopt;
    }

    if(query.next())
    {
        DataModel::StudentEnrollment se;
        se.student_enrollment_id = query.value("student_enrollment_id").toInt();
        se.enrollment_id = query.value("enrollment_id").toInt();
        se.class_id = query.value("class_id").toInt();
        se.year_id = query.value("year_id").toInt();
        se.start_date = query.value("start_date").toDate();
        se.end_date = query.value("end_date").toDate();
        se.status = DataModel::stringToStatus( query.value("status").toString());
        se.class_name = query.value("class_name").toString();
        se.section = query.value("class_section").toString();
        return se;
    }

    return std::nullopt;
}

QVector<DataModel::StudentEnrollment> DataAccess::StudentEnrollmentDAL::getAllStudentsEnrollments(int limit, int offset)
{
    QVector<DataModel::StudentEnrollment> students;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT se.student_enrollment_id, se.enrollment_id, se.class_id, se.year_id,
                        se.start_date, se.end_date, se.status,
                        p.person_id, p.first_name, p.second_name, p.third_name, p.fourth_name,
                        c.name AS class_name, c.section AS class_section
                    FROM student_enrollment se
                    JOIN enrollment e ON se.enrollment_id = e.enrollment_id
                    JOIN people p ON e.person_id = p.person_id
                    JOIN classes c ON se.class_id = c.class_id
                    ORDER BY se.student_enrollment_id
                    OFFSET :offset
                    LIMIT :limit
)")  ;

query.bindValue(":offset", offset);
query.bindValue(":limit", limit);

// if(!query.exec())
// {
//     qWarning() << "GET ALL STUDENTS ENROLLMENTS ERROR:" << query.lastError().text();
//     return enrollments;
// }

// while(query.next())
// {

// }
}











































