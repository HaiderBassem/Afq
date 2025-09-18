#include "ClassDAL.h"
#include"DatabaseManager/databaseManager.h"

#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlError>


std::optional<int> DataAccess::ClassDAL::createClass(int schoolId, const QString &name, const QString &section, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("INSERT INTO classes(school_id, name, section, year) VALUES(:schoolId, :name, :section, :year) RETURNING class_id ");

    query.bindValue(":schoolId", schoolId);
    query.bindValue(":name", name);
    query.bindValue(":section", section);
    query.bindValue(":year",year);

    if (query.exec() && query.next())
    {
        return query.value(0).toInt();
    }

    qWarning() << "ADD CLASS ERROR:" << query.lastError().text();
    return std::nullopt;
}

std::optional<DataModel::Class> DataAccess::ClassDAL::getClassById(int classId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE class_id = :classId");
    query.bindValue(":classId", classId);

    if(!query.exec())
    {
        qWarning()<<"GET CLASS BY ID ERROR:"<< query.lastError().text();
        return std::nullopt;
    }
    if(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.year_id = query.value("school_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year_id = query.value("year").toInt();
        return cls;
    }
    return std::nullopt;
}

bool DataAccess::ClassDAL::updateClass(int classId, const QString &name, const QString &section, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("UPDATE classes SET name = :name, section = :section, year = :year WHERE class_id = :classId");
    query.bindValue(":classId", classId);
    query.bindValue(":name",name);
    query.bindValue(":section", section);
    query.bindValue(":year", year);

    if(!query.exec())
    {
        qWarning() << "UPDATE CLASS ERROR:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

bool DataAccess::ClassDAL::deleteClass(int classId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("DELETE FROM classes WHERE class_id = :classId");
    query.bindValue(":classId", classId);

    if(!query.exec())
    {
        qWarning() << "DELETE CLASS ERROR:" << query.lastError().text();
        return false;
    }
    return query.numRowsAffected() > 0;
}

QVector<DataModel::Class> DataAccess::ClassDAL::getClassesBySchool(int schoolId)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE school_id = :schoolId");
    query.bindValue(":schoolId",schoolId);

    if(!query.exec())
    {

        qWarning()<<"GET CLASS BY SCHOOL ID ERROR:"<< query.lastError().text();
        return Classes;
    }

    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        //cls.school_id = query.value("school_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year_id = query.value("year").toInt();
        Classes.append(cls);
    }

    return Classes;
}

QVector<DataModel::Class> DataAccess::ClassDAL::getClassesByYear(int year)
{
    QVector<DataModel::Class> Classes;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare("SELECT * FROM classes WHERE year = :year");
    query.bindValue(":year",year);

    if(!query.exec())
    {

        qWarning()<<"GET CLASS BY YEAR ERROR:"<< query.lastError().text();
        return Classes;
    }

    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.school_id = query.value("school_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year_id = query.value("year").toInt();
        Classes.append(cls);
    }

    return Classes;
}

QVector<DataModel::Class> DataAccess::ClassDAL::searchClasses(const QString &searchTerm)
{
       QVector<DataModel::Class> Classes;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(SELECT class_id, school_id, name, section, year FROM classes
                    WHERE name ILIKE :search OR section ILIKE :search )");
    query.bindValue(":search","%" + searchTerm + "%" );

    if(!query.exec())
    {
        qWarning() << "SEARCH CLASSES ERROR:" << query.lastError().text();
        return Classes;
    }

    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.school_id = query.value("school_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year_id = query.value("year").toInt();
        Classes.append(cls);
    }

    return Classes;
}

QVector<DataModel::Class> DataAccess::ClassDAL::getAllClasses(int limit, int offset)
{

    QVector<DataModel::Class> Classes;

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(SELECT class_id, school_id, name, section, year FROM classes
                        OFFSET :offset LIMIT :limit )");


    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if(!query.exec())
    {
        qWarning() << "SEARCH CLASSES ERROR:" << query.lastError().text();
        return Classes;
    }

    while(query.next())
    {
        DataModel::Class cls;
        cls.class_id = query.value("class_id").toInt();
        cls.school_id = query.value("school_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.year_id = query.value("year").toInt();
        Classes.append(cls);
    }

    return Classes;
}

int DataAccess::ClassDAL::studentCount(int classId, int yearId)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT COUNT(se.student_enrollment_id)
                    FROM student_enrollment se
                    JOIN enrollment e ON se.enrollment_id = e.enrollment_id
                    WHERE se.class_id = :classId
                    se.year_id = :classId
                    e.role = 'Student'
                    se.status = 'Active'
)");

    query.bindValue(":classId",classId);
    query.bindValue(":yearId", yearId);

    if (!query.exec())
    {
        qWarning() << "STUDENT COUNT ERROR:" << query.lastError().text();
        return 0;
    }

    if(query.next())
        return query.value(0).toUInt();

    return 0;

}

int DataAccess::ClassDAL::subjectCount(int classId, int year)
{
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT COUNT(DISTINCT subject_id)
                    FROM course_offering
                    WHERE class_id = :classId
                    AND year_id = :yearId
)");

    query.bindValue(":classId", classId);
    query.bindValue(":yearId",year);

    if(!query.exec())
    {
        qWarning() << "SUBJECT COUNT ERROR:" << query.lastError().text();
        return 0;
    }
    if(query.next())
        return query.value(0).toInt();
    return 0;
}

QVector<DataModel::ClassSummary> DataAccess::ClassDAL::getclassSummary(int classId, int year)
{
        QVector<DataModel::ClassSummary> summaries;
    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
            c.class_id,
            c.name AS class_name,
            c.section,
            c.year AS academic_year,
            COUNT(DISTINCT se.student_enrollment_id) AS student_count,
            COUNT(DISTINCT co.subject_id) AS subject_count,
            ROUND(AVG(g.score::decimal / NULLIF(g.max_score, 0) * 100)::numeric, 2) AS class_average,
            ROUND(COALESCE(SUM(CASE WHEN a.status = 'Present' THEN 1 ELSE 0 END)::decimal /
                NULLIF(COUNT(a.attendance_id), 0), 0) * 100, 2) AS attendance_rate,
            CONCAT_WS(' ', NULLIF(t.first_name, ''), NULLIF(t.second_name, ''), NULLIF(t.third_name, '')) AS main_teacher
        FROM classes c
        LEFT JOIN student_enrollment se ON se.class_id = c.class_id AND se.year_id = c.year
        LEFT JOIN enrollment e ON e.enrollment_id = se.enrollment_id
        LEFT JOIN course_offering co ON co.class_id = c.class_id AND co.year_id = c.year
        LEFT JOIN grades g ON g.student_enrollment_id = se.student_enrollment_id AND g.offering_id = co.offering_id
        LEFT JOIN attendance a ON a.student_enrollment_id = se.student_enrollment_id AND a.offering_id = co.offering_id
        LEFT JOIN enrollment t_en ON t_en.enrollment_id = co.teacher_enrollment_id
        LEFT JOIN people t ON t.person_id = t_en.person_id
        WHERE c.class_id = :classId AND c.year = :yearId
        GROUP BY c.class_id, c.name, c.section, c.year, t.first_name, t.second_name, t.third_name
        ORDER BY c.name, c.section;
)");

    query.bindValue(":classId",classId);
    query.bindValue(":yearId", year);

    if (!query.exec())
    {
        qWarning() << "GET CLASS SUMMARY ERROR:" << query.lastError().text();
        return summaries;
    }


    while(query.next())
    {
        DataModel::ClassSummary cls;
        cls.class_id = query.value("class_id").toInt();
        cls.name = query.value("name").toString();
        cls.section = query.value("section").toString();
        cls.student_count = query.value("student_count").toInt();
        cls.subject_count = query.value("subject_count").toInt();
        cls.year_id = query.value("year").toInt();
        cls.class_average = query.value("class_average").toDouble();
        cls.attendance_rate = query.value("attendance_rate").toDouble();
        cls.main_teacher = query.value("main_teacher").toString();
        summaries.append(cls);
    }

    return summaries;
}

double DataAccess::ClassDAL::getClassAttendanceRate(int classId, int yearId)
{

    auto connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QSqlQuery query(db);

    query.prepare(R"(
                    SELECT
                    SUM(CASE WHEN a.status = 'Present' THEN 1 ELSE 0 END) AS present_count,
                    COUNT(*) AS total_count
                FROM attendance a
                JOIN student_enrollment se ON a.student_enrollment_id = se.student_enrollment_id
                WHERE se.class_id = :classId AND se.year_id = :yearId
)");

    query.bindValue(":classId",classId);
    query.bindValue(":yearId",yearId);
    if(!query.exec())
    {
        qWarning()<< "GET CLASS ATTENDANCE RATE ERROR:" << query.lastError().text();
        return 0.0;
    }

    if(query.next())
    {
        int present = query.value("present_count").toInt();
        int total = query.value("total_count").toInt();

        if(total == 0) return 0.0;
        return (static_cast<double>(present) / total) * 100.0;
    }

    return 0.0;
}








