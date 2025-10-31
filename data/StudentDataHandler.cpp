#include "StudentDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"
#include"PeopleDataHandler.h"
#include"GuardianDataHandler.h"
#include"DataModel/Student.h"
#include"DataModel/Enrollment.h"
#include"DataModel/StudentEnrollment.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QSqlDatabase>

#include<optional>
#include<vector>
#include<variant>
#include<iostream>
#include<iomanip>

std::optional<int> DataAccess::StudentDataHandler::addStudent(const DataModel::Student &student)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();


    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for adding student");
        return std::nullopt;
    }
    if (!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for adding student");
        return std::nullopt;
    }

    try
    {
        QSqlQuery chkPersonQuery(db);
        chkPersonQuery.prepare("SELECT person_id FROM people WHERE  = ? "); 
        chkPersonQuery.addBindValue(student.get_person_id());
        if(!chkPersonQuery.exec() && !chkPersonQuery.next())
        {
            qCritical() <<"\033[31m Person is not found with ID: \033[0m" << student.get_person_id();
            db.rollback();
            return std::nullopt;
        }

        QSqlQuery chkStudentQuery(db);
        chkStudentQuery.prepare(R"(
            SELECT enrollment_id FROM enrollment
            WHERE person_id  =  ? AND role = 0
)");

        chkStudentQuery.addBindValue(student.get_person_id());
        if(chkStudentQuery.exec() && chkStudentQuery.next())
        {
            qCritical() << "\033[31m Student already exists for person ID:" << student.get_person_id();
            db.rollback();
            return std::nullopt;
        }

        QSqlQuery enrollmentQuery(db);
        enrollmentQuery.prepare(R"(
            INSERT INTO enrollment (person_id, role, start_date, status)
            VALUES (?, 0, CURRENT_DATE, 1)   -- 0 = student role, 1 = active status
            RETURNING enrollment_id;
        )");


        enrollmentQuery.addBindValue(student.get_person_id());

        if(!enrollmentQuery.exec() && !enrollmentQuery.next())
        {
            qCritical() << "\033[31m Failed to insert enrollment:\033[0m" << enrollmentQuery.lastError().text();
            Logger::instance().error("Failed to insert enrollment: " + enrollmentQuery.lastError().text());
            db.rollback();
            return std::nullopt;
        }


        int enrollmentId = enrollmentQuery.value("enrollment_id").toInt();
        if(student.get_current_class_id() > 0 && student.get_current_year_id() > 0)
        {
            QSqlQuery studentEnrollmentQuery(db);
            studentEnrollmentQuery.prepare(R"(
            INSERT INTO student_enrollment (
                enrollment_id, class_id,year_id, start_date, status)
            VALUES(?, ?, CURRENT_DATE, 1)
            RETURNING student_enrollment_id
            )");

            studentEnrollmentQuery.addBindValue(enrollmentId);
            studentEnrollmentQuery.addBindValue(student.get_current_class_id());
            studentEnrollmentQuery.addBindValue(student.get_current_year_id()); // I need to set the year id from the user input

            if(!studentEnrollmentQuery.exec() && !studentEnrollmentQuery.next())
            {
                qCritical() << "\033[31m Failed to create student enrollment:\033[0m" << studentEnrollmentQuery.lastError().text();
                Logger::instance().error("Failed to create student enrollment: " + studentEnrollmentQuery.lastError().text());
                db.rollback();
                return std::nullopt;
            }


            int studentEnrollmenId = studentEnrollmentQuery.value("student_enrollment_id").toInt();
            Q_UNUSED(studentEnrollmenId);
            }

        return enrollmentId;

        }
            catch (const std::exception& e)
            {
                qCritical() <<"\033[31mException occurred while adding student: " <<e.what();
                Logger::instance().error("Exception occurred while adding student: " + QString(e.what()));
                db.rollback();
                return std::nullopt;
            }

}

bool DataAccess::StudentDataHandler::updateStudent(int studentId, const DataModel::Student &student)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for updating student");
        return false;
    }

    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for updating student");
        return false;
    }
try{
    QSqlQuery chkQuery(db);
    chkQuery.setForwardOnly(true);
    chkQuery.prepare(R"(
        SELECT e.enrollment_id
        FROM enrollment e
        WHERE e.enrollment_id = ? AND e.role = 0
    )");

    chkQuery.addBindValue(studentId);

    if(!chkQuery.exec() || !chkQuery.next())
    {
        qCritical() << "\033[31m Student not found with ID: \033[0m" << studentId;
        Logger::instance().error("Student not found with ID: " + QString::number(studentId));
        db.rollback();
        return false;
    }
    chkQuery.finish();

    QSqlQuery updateQuery(db);
    updateQuery.prepare(R"(
        UPDATE enrollment
        SET 
            end_date = ?,
            status = ?
        WHERE enrollment_id = ? AND role = 0
    )"); 

    updateQuery.addBindValue(student.get_graduation_date());
    updateQuery.addBindValue(static_cast<int>(student.get_status()));
    updateQuery.addBindValue(studentId);


    if(updateQuery.exec() && updateQuery.numRowsAffected() == 0)
    {
        qCritical() << "\033[31m No rows updated for student ID: \033[0m" << studentId;
        Logger::instance().error("No rows updated for student ID: " + QString::number(studentId));
        db.rollback();
        return false;
    }

    QSqlQuery chkEnrollmentQuery(db);
    chkEnrollmentQuery.setForwardOnly(true);
    chkEnrollmentQuery.prepare(R"(
        SELECT se.student_enrollment_id
        FROM student_enrollment se
        WHERE se.enrollment_id = ?
        AND (end_date IS NULL OR end_date > CURRENT_DATE)
        ORDER BY se.start_date DESC
        LIMIT 1
    )");
    chkEnrollmentQuery.addBindValue(studentId);
    std::optional<int> studentEnrollmentId;
    if(chkEnrollmentQuery.exec() && chkEnrollmentQuery.next())
    {
        studentEnrollmentId = chkEnrollmentQuery.value("student_enrollment_id").toInt();
    }
    if(student.get_current_class_id() > 0 && student.get_current_year_id() > 0)
    {
        if(studentEnrollmentId.has_value())
        {
            QSqlQuery updateEnrollmentQuery(db);
            updateEnrollmentQuery.prepare(R"(
                UPDATE student_enrollment
                SET class_id = ?,
                    year_id = ?,
                WHERE student_enrollment_id = ? 
            )");
            
            updateEnrollmentQuery.addBindValue(student.get_current_class_id());
            updateEnrollmentQuery.addBindValue(student.get_current_year_id());
            updateEnrollmentQuery.addBindValue(studentEnrollmentId.value());

            if(!updateEnrollmentQuery.exec() || updateEnrollmentQuery.numRowsAffected() == 0)
            {
                qCritical() << "\033[31m Failed to update student enrollment for student ID: \033[0m" << studentId;
                Logger::instance().error("Failed to update student enrollment for student ID: " + QString::number(studentId));
                db.rollback();
                return false;
            }
        }
        else
        {
            QSqlQuery insertStudentEnrollmentQuery(db);
            insertStudentEnrollmentQuery.prepare(R"(
                INSERT INTO student_enrollment
                (enrollment_id, class_id, year_id, start_date, status)
                VALUES (?, ?, ?, CURRENT_DATE, 1)
            )");
            insertStudentEnrollmentQuery.addBindValue(studentId);
            insertStudentEnrollmentQuery.addBindValue(student.get_current_class_id());
            insertStudentEnrollmentQuery.addBindValue(student.get_current_year_id());
            
            if(!insertStudentEnrollmentQuery.exec())
            {
                qCritical() << "\033[31m Failed to insert student enrollment for student ID: \033[0m" << studentId;
                Logger::instance().error("Failed to insert student enrollment for student ID: " + QString::number(studentId));
                db.rollback();
                return false;
            }

        }

        QSqlQuery updatePersonQuery(db);
        updatePersonQuery.prepare(R"(
            UPDATE people
            SET first_name = ?,
              second_name = ?,
              third_name = ?,
              fourth_name = ?,
              date_of_birth = ?,
              updated_at = CURRENT_TIMESTAMP
            WHERE person_id = (
                SELECT person_id FROM enrollment WHERE enrollment_id = ?
            )
        )");
        updatePersonQuery.addBindValue(student.get_first_name());
        updatePersonQuery.addBindValue(student.get_second_name());
        updatePersonQuery.addBindValue(student.get_third_name());
        updatePersonQuery.addBindValue(student.get_fourth_name());
        updatePersonQuery.addBindValue(student.get_date_of_birth());
        updatePersonQuery.addBindValue(studentId);
        if(!updatePersonQuery.exec() || updatePersonQuery.numRowsAffected() == 0)
        {
            qCritical() << "\033[31m Failed to update person details for student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to update person details for student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for updating student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to commit transaction for updating student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        qInfo() << "\033[32m Successfully updated student with ID:\033[0m" << studentId;

        return true;
}



}
catch (const std::exception& e)
{
    qCritical() <<"\033[31mException occurred while checking student existence: " <<e.what();
    Logger::instance().error("Exception occurred while checking student existence: " + QString(e.what()));
    db.rollback();  
}
    return false;
}

bool DataAccess::StudentDataHandler::deleteStudent(int studentId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for deleting student");
        return false;
    }
    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for deleting student");
        return false;
    }
    try{
        std::optional<int> personId = std::nullopt;
        QSqlQuery getPersonQuery(db);
        getPersonQuery.setForwardOnly(true);
        getPersonQuery.prepare(R"(
            SELECT person_id FROM enrollment
            WHERE enrollment_id = ? AND role = 0
        )");
        getPersonQuery.addBindValue(studentId);

        if(getPersonQuery.exec() && getPersonQuery.next())
        {
            personId = getPersonQuery.value("person_id").toInt();
        }
        else
        {
            qCritical() << "\033[31m Student not found with ID: \033[0m" << studentId;
            Logger::instance().error("Student not found with ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        getPersonQuery.finish();

        QString studentNumber;
        QSqlQuery getStudentQuery(db);
        getStudentQuery.setForwardOnly(true);
        getStudentQuery.prepare(R"(
            SELECT student_number FROM students
            WHERE enrollment_id = ?
        )");
        getStudentQuery.addBindValue(studentId);

        if(getStudentQuery.exec() && getStudentQuery.next())
        {
            studentNumber = getStudentQuery.value("student_number").toString();
        }
        getStudentQuery.finish();

        QSqlQuery deleteEnrollmentQuery(db);
        deleteEnrollmentQuery.prepare(R"(
            DELETE FROM enrollment
            WHERE enrollment_id = ? AND role = 0
        )");
        deleteEnrollmentQuery.addBindValue(studentId);

        if(!deleteEnrollmentQuery.exec() || deleteEnrollmentQuery.numRowsAffected() == 0)
        {
            qCritical() << "\033[31m Failed to delete student with ID: \033[0m" << studentId;
            Logger::instance().error("Failed to delete student with ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }


        if(personId.has_value())
        {
            QSqlQuery chkPersonUsageQuery(db);
            chkPersonUsageQuery.setForwardOnly(true);
            chkPersonUsageQuery.prepare(R"(
                SELECT COUNT(*) AS usage_count FROM enrollment
                WHERE person_id = ?
            )");
            chkPersonUsageQuery.addBindValue(personId.value());

            if(chkPersonUsageQuery.exec() && chkPersonUsageQuery.next())
            {
                int usageCount = chkPersonUsageQuery.value("usage_count").toInt();
                if(usageCount == 0)
                {
                    QSqlQuery deleteStudentGuardiansQuery(db);
                    deleteStudentGuardiansQuery.prepare(R"(
                        DELETE FROM guardians
                        WHERE student_person_id = ?
                    )");
                    deleteStudentGuardiansQuery.addBindValue(personId.value());
                    if(!deleteStudentGuardiansQuery.exec())
                    {
                        qCritical() << "\033[31m Failed to delete guardians for student person ID: \033[0m" << personId.value();
                        Logger::instance().error("Failed to delete guardians for student person ID: " + QString::number(personId.value()));
                    }

                    QSqlQuery deleteOrphanGuardiansQuery(db);
                    deleteOrphanGuardiansQuery.prepare(R"(
                        DELETE FROM guardians 
                        WHERE guardian_id IN (
                            SELECT sg.guardian_id FROM student_guardians sg
                            WHERE sg.student_person_id = ?
                            )
                            AND NOT EXISTS (
                                SELECT 1 FROM student_guardians sg2
                                WHERE sg2.guardian_id = guardians.guardian_id
                                AND sg2.student_person_id != ?))");
                    deleteOrphanGuardiansQuery.addBindValue(personId.value());
                    deleteOrphanGuardiansQuery.addBindValue(personId.value());

                    if(!deleteOrphanGuardiansQuery.exec())
                    {
                        qCritical() << "\033[31m Failed to delete orphan guardians for student person ID: \033[0m" << personId.value();
                        Logger::instance().error("Failed to delete orphan guardians for student person ID: " + QString::number(personId.value()));
                    }
                }
            }
        }

        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for deleting student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to commit transaction for deleting student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
 qInfo() << "\033[32m Successfully deleted student with ID:\033[0m" << studentId 
                << "Number:" << studentNumber;
    return true;
}
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while deleting student: " <<e.what();
        Logger::instance().error("Exception occurred while deleting student: " + QString(e.what()));
        db.rollback();
        return false;
    }
}

DataModel::Student DataAccess::StudentDataHandler::getStudentById(int studentId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for getting student by ID");
        return DataModel::Student();
    }
    try{
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
        SELECT 
            e.enrollment_id AS student_id,
            p.person_id,
            se.student_enrollment_id,
            
            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            p.date_of_birth,
            
            e.enrollment_number AS student_number,
            e.start_date AS enrollment_date,
            e.end_date AS graduation_date,
            e.status AS enrollment_status,

            se.class_id AS current_class_id,
            se.year_id AS current_year_id,
            se.final_average AS current_average,
            se.rank_in_class AS current_rank,
            se.repeat_count,
            se.is_graduated,
            se.is_repeated,
            se.is_ministerial_exam,
            se.is_eligible_for_exam,

            c.name AS class_name,
            c.section,
            c.grade_level,
            ay.name AS year_name
        FROM enrollment e
        LEFT JOIN people p ON e.person_id = p.person_id
        LEFT JOIN student_enrollment se 
            ON e.enrollment_id = se.enrollment_id 
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
        LEFT JOIN classes c ON se.class_id = c.class_id
        LEFT JOIN academic_year ay ON se.year_id = ay.year_id
        WHERE e.enrollment_id = ? 
        AND e.role = 0
        ORDER BY se.start_date DESC
        LIMIT 1;

            )");

            query.addBindValue(studentId);
            if(!query.exec() || !query.next())
            {
                qCritical() << "\033[31m Student not found with ID: \033[0m" << studentId;
                Logger::instance().error("Student not found with ID: " + QString::number(studentId));
                return DataModel::Student();
            }

            auto student = createStudentFromQuery(query);
            return student;

}
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while getting student by ID: " <<e.what();
        Logger::instance().error("Exception occurred while getting student by ID: " + QString(e.what()));
        return DataModel::Student();
    }
}
QVector<DataModel::Student> DataAccess::StudentDataHandler::getAllStudents()
{
    QVector<DataModel::Student> students;
    

    const auto& connWrapper = DatabaseManager::instance().getConnection();
    if (!connWrapper) {
        qCritical() << "Database connection wrapper is null";
        return students;
    }
    
    const QSqlDatabase& db = connWrapper->database();
    if (!db.isOpen()) {
        qCritical() << "Database connection is not open";
        return students;
    }


    QSqlQuery query(db);
    

    query.prepare(R"(
        SELECT 
            e.enrollment_id,
            e.person_id,
            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            e.status,
            e.start_date AS enrollment_date,
            c.name AS class_name,
            se.final_average AS current_average,
            se.rank_in_class AS current_rank,
            se.is_eligible_for_exam,
            ay.name AS academic_year_name,
            es.name AS stage_name
        FROM enrollment e
        INNER JOIN people p ON e.person_id = p.person_id
        LEFT JOIN student_enrollment se ON e.enrollment_id = se.enrollment_id
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
        LEFT JOIN classes c ON se.class_id = c.class_id
        LEFT JOIN academic_year ay ON se.year_id = ay.year_id
        LEFT JOIN education_stages es ON c.stage_id = es.stage_id
        WHERE e.role = 0
        ORDER BY e.enrollment_number ASC,
                 p.first_name ASC,
                 p.second_name ASC,
                 e.start_date DESC;
    )");

    if (!query.exec()) {
        qCritical() << "Query execution failed:" << query.lastError().text();
        return students;
    }

    int estimatedSize = std::min(query.size(), 1000); 
    students.reserve(estimatedSize > 0 ? estimatedSize : 100);

    while (query.next()) 
    {
        students.append(createStudentFromQuery(query));
    }

 
    if (students.capacity() > students.size() * 1.5) 
    {
        students.squeeze();
    }

    qInfo() << "Retrieved" << students.size() << "students, Memory usage optimized";
    return students;
}


std::optional<int> DataAccess::StudentDataHandler::enrollStudent(int studentId, int classId, int yearId, const QDate &startDate, DataModel::StudentStatus status)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for enrolling student");
        return std::nullopt;
    }
    if (!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for enrolling student");
        return std::nullopt;
    }

    try{
        QSqlQuery chkActiveEnrollmentQuery(db);
        chkActiveEnrollmentQuery.setForwardOnly(true);
        chkActiveEnrollmentQuery.prepare(R"(
            SELECT 1 FROM student_enrollment
            WHERE enrollment_id = ?
            AND year_id = ?
            AND status = 1 -- Active status
            AND (end_date IS NULL OR end_date > CURRENT_DATE) 
        )");

        chkActiveEnrollmentQuery.addBindValue(studentId);
        chkActiveEnrollmentQuery.addBindValue(yearId);

        if(chkActiveEnrollmentQuery.exec() && chkActiveEnrollmentQuery.next())
        {
            qCritical() << "\033[31m Student is already actively enrolled for the specified year. Student ID: \033[0m" << studentId;
            Logger::instance().error("Student is already actively enrolled for the specified year. Student ID: " + QString::number(studentId));
            db.rollback();
            return std::nullopt;
        }

        // end previous enrollment if exists
        QSqlQuery endPreviousEnrollmentQuery(db);
        endPreviousEnrollmentQuery.prepare(R"(
            UPDATE student_enrollment
            SET end_date = ?
                status = 3 -- Transferred status
                updated_at = CURRENT_TIMESTAMP
            WHERE enrollment_id = ?
            AND status = 1 -- Active status
            AND (end_date IS NULL OR end_date > CURRENT_DATE)
        )");
        endPreviousEnrollmentQuery.addBindValue(startDate.addDays(-1));
        endPreviousEnrollmentQuery.addBindValue(studentId);
        if(!endPreviousEnrollmentQuery.exec())
        {
            qCritical() << "\033[31m Failed to end previous enrollment for student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to end previous enrollment for student ID: " + QString::number(studentId));
            db.rollback();
            return std::nullopt;
        }


        int studentEnrollmentStatus = static_cast<int>(status);
        QSqlQuery insertEnrollmentQuery(db);
        insertEnrollmentQuery.prepare(R"(
            INSERT INTO student_enrollment
            (enrollment_id, class_id, year_id, start_date, status)
            VALUES (?, ?, ?, CURRENT_DATE, ?)
            RETURNING student_enrollment_id
        )");
        insertEnrollmentQuery.addBindValue(studentId);
        insertEnrollmentQuery.addBindValue(classId);
        insertEnrollmentQuery.addBindValue(yearId);
        insertEnrollmentQuery.addBindValue(studentEnrollmentStatus);
        if(!insertEnrollmentQuery.exec() || !insertEnrollmentQuery.next())
        {
            qCritical() << "\033[31m Failed to enroll student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to enroll student ID: " + QString::number(studentId));
            db.rollback();
            return std::nullopt;
        }
        int studentEnrollmentId = insertEnrollmentQuery.value("student_enrollment_id").toInt();

        QSqlQuery updateStudentStatusQuery(db);
        updateStudentStatusQuery.prepare(R"(
            UPDATE enrollment
                SET status = ?
                updated_at = CURRENT_TIMESTAMP
            WHERE enrollment_id = ? AND role = 0
        )");

        int enrollmentStatus = static_cast<int>(status);
        updateStudentStatusQuery.addBindValue(enrollmentStatus);
        updateStudentStatusQuery.addBindValue(studentId);
        if(!updateStudentStatusQuery.exec() || updateStudentStatusQuery.numRowsAffected() == 0)
        {
            qCritical() << "\033[31m Failed to update enrollment status for student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to update enrollment status for student ID: " + QString::number(studentId));
            db.rollback();
            return std::nullopt;
        }
        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for enrolling student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to commit transaction for enrolling student ID: " + QString::number(studentId));
            db.rollback();
            return std::nullopt;
        }
        qInfo() << "\033[32m Successfully enrolled student with ID:\033[0m" << studentId
                << "to class ID:" << classId
                << "for academic year ID:" << yearId;
        return studentEnrollmentId;
    }
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while enrolling student: " <<e.what();
        Logger::instance().error("Exception occurred while enrolling student: " + QString(e.what()));
        db.rollback();
    }

    return std::nullopt;
}

bool DataAccess::StudentDataHandler::reenrollStudent(int studentId, int classId, int yearId, const QString &section, const QDate &reenrollDate)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for reenrolling student");
        return false;
    }
    if (!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for reenrolling student");    
        return false;
    }

    try{
        QSqlQuery getLastEnrollmentQuery(db);
        getLastEnrollmentQuery.setForwardOnly(true);
        getLastEnrollmentQuery.prepare(R"(
            SELECT student_enrollment_id, 
            class_id, year_id, final_average,
            rank_in_class
            repeat_count, is_graduated
            FROM student_enrollment
            WHERE enrollment_id = ?
            ORDER BY start_date DESC
            LIMIT 1
        )");

        getLastEnrollmentQuery.addBindValue(studentId);
        int lastClassId = -1;
        int lastYearId = -1;
        double lastAverage = 0.0;
        int lastRank = 0;
        int lastRepeatCount = 0;
        bool wasGraduated = false;
        if(getLastEnrollmentQuery.exec() && getLastEnrollmentQuery.next())
        {
            lastClassId = getLastEnrollmentQuery.value("class_id").toInt();
            lastYearId = getLastEnrollmentQuery.value("year_id").toInt();
            lastAverage = getLastEnrollmentQuery.value("final_average").toDouble();
            lastRank = getLastEnrollmentQuery.value("rank_in_class").toInt();
            lastRepeatCount = getLastEnrollmentQuery.value("repeat_count").toInt();
            wasGraduated = getLastEnrollmentQuery.value("is_graduated").toBool();
        }
        else
        {
            qCritical() << "\033[31m No previous enrollment found for student ID: \033[0m" << studentId;
            Logger::instance().error("No previous enrollment found for student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        getLastEnrollmentQuery.finish();
        // Check if the student was graduated
        if (wasGraduated) 
        {
            qCritical() << "\033[31m Cannot reenroll a graduated student\033[0m";
            Logger::instance().error("Cannot reenroll a graduated student");
            db.rollback();
            return false;
        }
    

        bool shouldRepeat  = calculateIfShouldRepeat(lastClassId, classId, lastAverage, db);
        if(shouldRepeat && lastClassId == classId)
        {
            lastRepeatCount += 1;
        }

        QSqlQuery insertReenrollmentQuery(db);
        insertReenrollmentQuery.prepare(R"(
            INSERT INTO student_enrollment
            (enrollment_id, class_id, year_id, start_date, status)
            VALUES (?, ?, ?, ?, 1)
            RETURNING student_enrollment_id
        )");
        insertReenrollmentQuery.addBindValue(studentId);
        insertReenrollmentQuery.addBindValue(classId);
        insertReenrollmentQuery.addBindValue(yearId);
        insertReenrollmentQuery.addBindValue(reenrollDate);
        if(!insertReenrollmentQuery.exec() || !insertReenrollmentQuery.next())
        {
            qCritical() << "\033[31m Failed to reenroll student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to reenroll student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        int newStudentEnrollmentId = insertReenrollmentQuery.value("student_enrollment_id").toInt();

             QSqlQuery updateStudentStatusQuery(db);
        updateStudentStatusQuery.prepare(R"(
            UPDATE enrollment 
            SET status = 1,  -- Active
                updated_at = CURRENT_TIMESTAMP
            WHERE enrollment_id = ?
        )");
        updateStudentStatusQuery.addBindValue(studentId);
        if(!updateStudentStatusQuery.exec() || updateStudentStatusQuery.numRowsAffected() == 0)
        {
            qCritical() << "\033[31m Failed to update enrollment status for student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to update enrollment status for student ID: " + QString::number(studentId));
            db.rollback();  
            return false;
        }

        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for reenrolling student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to commit transaction for reenrolling student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        qInfo() << "\033[32m Successfully reenrolled student with ID:\033[0m" << studentId
                << "to class ID:" << classId
                << "for academic year ID:" << yearId;
        return true;






    return false;
}catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while reenrolling student: " <<e.what();
        Logger::instance().error("Exception occurred while reenrolling student: " + QString(e.what()));
        db.rollback();
    }
    return false;

}





bool DataAccess::StudentDataHandler::updateStudentsAcademicInfo(int classId, int yearId)
{

    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for updating student academic info");
        return false;
    }


    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction\033[0m";
        Logger::instance().error("Failed to start transaction for updating student academic info");
        return false;
    }

    try{

        calculateRankForStudentInClass(classId, yearId);

        QSqlQuery getAvgsQuery(db);
        getAvgsQuery.setForwardOnly(true);
        getAvgsQuery.prepare(R"(
            SELECT se.student_enrollment_id, se.final_average
            FROM student_enrollment se
            WHERE se.class_id = ?
            AND se.year_id = ?
            AND se.status = 1 -- Active status only
        )");
        getAvgsQuery.addBindValue(classId);
        getAvgsQuery.addBindValue(yearId);


        if(!getAvgsQuery.exec())
        {
            qCritical() << "\033[31m Failed to get student averages for class ID: \033[0m" << classId;
            Logger::instance().error("Failed to get student averages for class ID: " + QString::number(classId));
            db.rollback();
            return false;
        }
        std::vector<std::pair<int, bool>> isElegibleList;
        while(getAvgsQuery.next())
        {
            isElegibleList.push_back({
                getAvgsQuery.value("student_enrollment_id").toInt(),
                getAvgsQuery.value("final_average").toDouble() >= 50.0
            });   
        }
        
        getAvgsQuery.finish();
        QSqlQuery updateElegiblityQuery(db);
        updateElegiblityQuery.prepare(R"(
            UPDATE student_enrollment
            SET
                is_eligible_for_exam = ?,
                updated_at = CURRENT_TIMESTAMP
            WHERE student_enrollment_id = ?
        )");

        for(const auto& [id, isElegible] : isElegibleList)
        {
            updateElegiblityQuery.bindValue(0, isElegible);
            updateElegiblityQuery.bindValue(1, id);
            if(!updateElegiblityQuery.exec() || updateElegiblityQuery.numRowsAffected() == 0)
            {
                qCritical() << "\033[31m Failed to update eligibility for student enrollment ID: \033[0m" << id 
                            << "Class ID:" << classId << "Year ID:" 
                            << yearId <<"\nErorr: "<< updateElegiblityQuery.lastError().text();
                db.rollback();
                return false;
            }
        }

    }
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while updating student academic info: " <<e.what();
        Logger::instance().error("Exception occurred while updating student academic info: " + QString(e.what()));
        db.rollback();
        return false;
    }

    return false;
}

bool DataAccess::StudentDataHandler::withdrawStudent(int studentId, int yearId, const QString &reason)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

if (!db.isOpen())
{
    qCritical() << "\033[31m Database connection is not open\033[0m";
    Logger::instance().error("Database connection is not open for withdrawing student");
    return false;
}

if (!db.transaction())
{
    qCritical() << "\033[31m Failed to start transaction\033[0m";
    Logger::instance().error("Failed to start transaction for withdrawing student");
    return false;
}

    if(!isStudentActive(studentId, db)) 
    {
        db.rollback();
        return false;
    }

    try{

        QSqlQuery updateEnrollmentQuery(db);
        updateEnrollmentQuery.prepare(R"(
            UPDATE student_enrollment
            SET end_date = CURRENT_DATE,
                status = 4 -- withdrawn,
                updated_at = CURRENT_TIMESTAMP
                WHERE student_enrollment_id = ? AND year_id = ?
        )");
        updateEnrollmentQuery.addBindValue(studentId);
        updateEnrollmentQuery.addBindValue(yearId);


        if(!updateEnrollmentQuery.exec() || updateEnrollmentQuery.numRowsAffected() == 0)
        {
            qCritical() << "\033[31m Failed to withdraw student ID: \033[0m" << studentId << "for year ID:" << yearId;
            Logger::instance().error("Failed to withdraw student ID: " + QString::number(studentId) + " for year ID: " + QString::number(yearId));
            db.rollback();
            return false;
        }
        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for withdrawing student ID: \033[0m" << studentId;
            Logger::instance().error("Failed to commit transaction for withdrawing student ID: " + QString::number(studentId));
            db.rollback();
            return false;
        }
        qInfo() << "\033[32m Successfully withdrew student with ID:\033[0m" << studentId 
                << "from academic year ID:" << yearId;
        return true;
    }
    catch (const std::exception& e)
    {
        qCritical() <<"\033[31mException occurred while withdrawing student: " <<e.what();
        Logger::instance().error("Exception occurred while withdrawing student: " + QString(e.what()));
        db.rollback();
    }
    return false;
}

bool DataAccess::StudentDataHandler::transferStudent(int studentId, int yearId, const QString& reason)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

if (!db.isOpen())
{
    qCritical() << "\033[31m Database connection is not open\033[0m";
    Logger::instance().error("Database connection is not open for transferring student");   
    return false;
}

if (!db.transaction())
{
    qCritical() << "\033[31m Failed to start transaction\033[0m";
    Logger::instance().error("Failed to start transaction for transferring student");
    db.rollback();
    return false;
}

if(!StudentDataHandler::isStudentExists(studentId, db))
{
    db.rollback();
    return false;
}

try{
    QSqlQuery updateEnrollmentQuery(db);
    updateEnrollmentQuery.prepare(R"(
        UPDATE student_enrollment
        SET class_id =?,
            status = 3, -- transferred
            notes = ?,
            updated_at = CURRENT_TIMESTAMP
            WHERE enrollment_id = ? AND year_id = ?
        )");

    updateEnrollmentQuery.addBindValue(studentId);
    updateEnrollmentQuery.addBindValue(yearId);
    updateEnrollmentQuery.addBindValue(reason);
    if(!updateEnrollmentQuery.exec() || updateEnrollmentQuery.numRowsAffected() == 0)
    {
        qCritical() << "\033[31m Failed to transfer student ID: \033[0m" << studentId << "for year ID:" << yearId;
        Logger::instance().error("Failed to transfer student ID: " + QString::number(studentId) + " for year ID: " + QString::number(yearId));
        db.rollback();
        return false;
    }
    if(!db.commit())
    {
        qCritical() << "\033[31m Failed to commit transaction for transferring student ID: \033[0m" << studentId;
        Logger::instance().error("Failed to commit transaction for transferring student ID: " + QString::number(studentId));
        db.rollback();
        return false;
    }
}
catch (const std::exception& e)
{
    qCritical() <<"\033[31mException occurred while transferring student: " <<e.what();
    Logger::instance().error("Exception occurred while transferring student: " + QString(e.what()));
    db.rollback();
    return false;
}
return true;
}

bool DataAccess::StudentDataHandler::graduateStudent(int studentId, int classId, int yearId, const QDate &graduationDate)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.open())
    {
        std::cerr << "\033[31m Database connection is not open \033[0m" << db.lastError().text().toStdString();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return false;
    }

    if(!db.transaction())
    {
        std::cerr << "\033[31m Failed to start transaction for graduation \033[0m" << db.lastError().text().toStdString();
        Logger::instance().error("Failed to start transaction " + db.lastError().text());
        return false;
    }


    if(!evaluateAllSubjectsAbove50(studentId, classId, yearId, db))
    {
        db.rollback();
        return false;
    }

    int studentEnrollmentId = getStudentEnrollmentById(studentId).enrollment_id; // student id = student enrollment id 
    // TODO: check the student at end level of studying
   try{
    QSqlQuery updateEnrollmentQuery(db);
    updateEnrollmentQuery.prepare(R"(
        UPDATE student_enrollment
            SET status = 2, -- graduate
                end_date = ?, 
                is_graduated = true,
                updated_at = CURRENT_TIMESTAMP,
                notes = COALESCE(notes, '') || ' [GRADUATED on ' || ? || ']'

                WHERE student_enrollment_id = ?
        )");
        updateEnrollmentQuery.addBindValue(graduationDate);
        updateEnrollmentQuery.addBindValue(graduationDate.toString("yyyy-MM-dd"));
        updateEnrollmentQuery.addBindValue(studentEnrollmentId);

        if(!updateEnrollmentQuery.exec())
        {
            std::cerr << "\033[31m Failed to update student enrollment ID: " << studentEnrollmentId << updateEnrollmentQuery.lastError().text().toStdString();
            Logger::instance().error("Failed to update student enrollment: " + updateEnrollmentQuery.lastError().text());
            db.rollback();
            return false;
        }

        QSqlQuery updateStudentStatusQuery(db);
        updateStudentStatusQuery.prepare(R"(
            UPDATE enrollment
            SET status = 2, -- Graduated
                end_date = ?,
                updated_at = CURRENT_TIMESTAMP
            WHERE enrollment_id = ?
            )");
            updateStudentStatusQuery.addBindValue(graduationDate);
            updateStudentStatusQuery.addBindValue(studentId);

            if(!updateStudentStatusQuery.exec())
            {
                qCritical() <<"\033[31m Failed to update student status:\033[0m" << updateStudentStatusQuery.lastError().text();
                Logger::instance().error("Failed to update student status " + updateStudentStatusQuery.lastError().text());
                db.rollback();
                return false;
            }
            
            // TODO: get the certification
            // TODO: update the class statistics
            
            if(!db.commit())
            {
                qCritical() << "\033[31m Failed to commit transaction:\033[0m" << db.lastError().text();
                Logger::instance().error("Failed to commit transaction: " + db.lastError().text());
                db.rollback();
                return false;
            }

             qInfo() << "\033[32m Successfully graduated student with ID:\033[0m" << studentId 
                << "from class:" << classId 
                << "year:" << yearId
                << "on:" << graduationDate.toString("yyyy-MM-dd");
        }
        catch(const std::exception& e)
        {
            qCritical() << "\033[31m Exception occurred while graduating student:\033[0m" << e.what();
            Logger::instance().error("Exception occurred while graduating student: " + QString(e.what()));
            db.rollback();
            return false;
        }
    return true;
}

QVector<DataModel::Student> DataAccess::StudentDataHandler::searchStudentsByName(const QString &fName, const QString &sName, const QString &tName, const QString &ftName, std::optional<int> classId, const QString &section, int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    QVector<DataModel::Student> students;
    if(!db.open())
    {
        std::cerr << "\033[31m Database connection is not open \033[0m" << db.lastError().text().toStdString();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return QVector<DataModel::Student>();
    }

    try 
    {
        // TODO: add total absences to database and here 
        QString queryStr = R"(
            SELECT 
                e.enrollment_id AS student_id,
                e.enrollment_number AS student_number,
                p.person_id,
                p.first_name,
                p.second_name,
                p.third_name,
                p.fourth_name,
                p.gender,
                p.date_of_birth,
                
                -- e.start_date AS enrollment_date,
                -- e.end_date AS graduation_date, 

                se.class_id AS current_class_id,
                se.year_id AS current_year_id,
                
                e.status AS enrollment_status,
                se.final_average AS current_average,
                se.rank_in_class AS current_rank,
               -- se.total_absences,
                
                c.name AS class_name,
                ay.name AS year_name
            FROM enrollment e
            JOIN people p ON e.person_id = p.person_id
            LEFT JOIN student_enrollment se ON e.enrollment_id = se.enrollment_id
            AND se.status =1
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
            LEFT JOIN classes c ON se.class_id = c.class_id 
            LEFT JOIN academic_year ay ON se.year_id = ay.year_id
            WHERE e.role = 0
            LIMIT 100
        )";
        QVector<QString> conditions;
        QVector<QVariant> bindValues;

        // for strings conditions
        auto addStringCondition = [&](const QString& value, const QString& condition)
        {
            if(!value.isEmpty())
            {
                conditions << condition;
                bindValues <<   "%" + value + "%";
            }
        };
        /// for int conditions
        auto addIntConditions = [&](std::optional<int> value, const QString& condition)
        {
            if(value.has_value() && value.value() > 0)
            {
                conditions << condition;
                bindValues << value.value();
            } 
        };

        addStringCondition(fName, "p.first_name LIKE ?");
        addStringCondition(sName, "p.second_name LIKE ?");
        addStringCondition(tName, "p.third_name LIKE ?");
        addStringCondition(ftName, "p.fourth_name LIKE ?");
        addIntConditions(classId, "se.class_id = ?");

        if(!section.isEmpty())
        {
            conditions << "c.section ILIKE ?";
            bindValues << "%" + section + "%";
        }
        if(yearId > 0)
        {
            conditions << "se.year_id = ?";
            bindValues << yearId;
        }
        if(!conditions.isEmpty())
        {
            queryStr += " AND " + conditions.join(" AND ");
        }

        queryStr += " ORDER BY p.first_name, p.second_name, p.third_name";

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryStr);

        for(const QVariant& value : bindValues)
        {
            query.addBindValue(value);
        }

        if(!query.exec())
        {
            qCritical() << "\033[31m Failed to search students by name:\033[0m" << query.lastError().text();
            Logger::instance().error("Failed to search students by name: " + query.lastError().text());
            return QVector<DataModel::Student>();
        }
     
        while(query.next())
        {
            students.append(createStudentFromQuery(query));
        }
        
        qInfo() << "\033[32m Found" << students.size() << "students matching search criteria\033[0m";
        return students;
    }
    catch(const std::exception& e)
    {
               qCritical() << "\033[31m Exception occurred while searching students by name:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while searching students by name: " + QString(e.what()));
        return QVector<DataModel::Student>();
    }


    return  QVector<DataModel::Student>();
}

DataModel::Student DataAccess::StudentDataHandler::searchStudentByStudentNumber(const QString &studentNumber)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();
    DataModel::Student student;

    if(!db.open())
    {
        std::cerr << "\033[31m Database connection is not open \033[0m" << db.lastError().text().toStdString();
        Logger::instance().error("Database connection is not open " + db.lastError().text());
        return DataModel::Student();
    }
    try
    {
QString queryStr = R"(
    SELECT 
        e.enrollment_id AS student_id,
        p.first_name,
        p.second_name,
        p.third_name,
        p.fourth_name,
        p.gender,
        p.date_of_birth,
        se.class_id AS current_class_id,
        se.year_id AS current_year_id,
        e.status AS enrollment_status,
        se.final_average AS current_average,
        se.rank_in_class AS current_rank,
        c.name AS class_name,
        ay.name AS year_name
    FROM enrollment e 
    JOIN people p ON e.person_id = p.person_id
    LEFT JOIN student_enrollment se ON e.enrollment_id = se.enrollment_id
        AND se.status = 1
        AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
    LEFT JOIN classes c ON se.class_id = c.class_id 
    LEFT JOIN academic_year ay ON se.year_id = ay.year_id 
    WHERE e.enrollment_number = ? AND e.role = 0  
    LIMIT 1
)";


         QSqlQuery query(db);
         query.setForwardOnly(true);
        query.prepare(queryStr);
        query.addBindValue(studentNumber);

        if(!query.exec())
        {
            qCritical() << "\033[31m Failed to search students by name:\033[0m" << query.lastError().text();
            Logger::instance().error("Failed to search students by name: " + query.lastError().text());
            return DataModel::Student();
        }
        while(query.next())
        {
            student = createStudentFromQuery(query);
        }
        qInfo() << "\033[32m Found student matching search criteria\033[0m";
        return student;

    }
    catch(const std::exception& e)
    {
               qCritical() << "\033[31m Exception occurred while searching students by name:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while searching students by name: " + QString(e.what()));
        return DataModel::Student();
    }

    return DataModel::Student();
}

QVector<DataModel::Student> DataAccess::StudentDataHandler::getStudentsByClass(const QString &className, int yearId, const QString &section)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();


    if(!db.isOpen())
    {
        qCritical() <<"\033[31m Database isn't open. \033[0m" <<db.lastError().text();
        Logger::instance().error("Database isn't open. " + db.lastError().text());
        return QVector<DataModel::Student>(); 
    }

    try
    {
        QString queryStr = R"(
            SELECT DISTINCT
                e.enrollment_id AS student_id,
                e.enrollment_number AS student_number,
                se.student_enrollment_id,
                p.person_id,
                p.first_name,
                p.second_name,
                p.third_name,
                p.fourth_name,
                p.gender,
                p.date_of_birth,

                --e.start_date AS enrollment_date,
                --e.end_date AS graduation_date,
                
                se.class_id AS current_class_id,
                se.year_id AS current_year_id,
                e.status AS enrollment_status,
                se.final_average AS current_average,
                se.rank_in_class AS current_rank,
                se.is_eligible_for_exam,
                c.name AS class_name,
                c.section AS class_section,
                c.grade_level,
                ay.name AS year_name,
                es.name AS stage_name
            FROM enrollment e 
            JOIN people USING (person_id)
            JOIN student_enrollment se USING (enrollment_id)
            JOIN classes c USING (class_id)
            JOIN academic_year ay USING(year_id)
            JOIN education_stage es USING (stage_id)
            WHERE e.role = 0
            AND se.status = 1
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)

        )";


        std::vector<std::pair<std::string, std::variant<int, std::string>>> conditionsAndValues;
        
        auto addCondition = [&](const std::string& condition, const std::variant<int, std::string>& value)
        {
            conditionsAndValues.push_back({condition, value});
        };
        
        if(!className.isEmpty()) 
            addCondition("c.name LIKE ?", std::variant<int, std::string>{className.toStdString() + "%"});
        if(!section.isEmpty())
            addCondition("c.section LIKE ?", std::variant<int, std::string>{section.toStdString() + "%"});
        if(yearId > 0)
            addCondition("se.year_id LIKE ?", std::variant<int, std::string>{yearId});

        if(!conditionsAndValues.empty())
        {
            QStringList condList;
            for(const auto& [condStr, _] : conditionsAndValues)
                condList.append(QString::fromStdString(condStr));
            queryStr += " AND " + condList.join(" AND ");
        }

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(queryStr);

        for(const auto& [_, value] : conditionsAndValues)
        {
            std::visit([&query](auto&& arg)
            {
                using T = std::decay_t<decltype(arg)>; //? declear the type of arg
                if constexpr (std::is_same_v<T,std::string>)
                    query.addBindValue(QString::fromStdString(arg));
                else
                    query.addBindValue(arg);
            }, value);
        }

        if(!query.exec())
        {
            qCritical() <<"\033[31m Failed to get students by class\0330m" <<query.lastError().text();
            Logger::instance().error("Failed to get stuednts by class " + query.lastError().text());
            return QVector<DataModel::Student>();
        }
        QVector<DataModel::Student> students;
        while(query.next())
        {
            students.append(createStudentFromQuery(query));
        }
          qInfo() << "\033[32m Found" << students.size() << "students in class search:\033[0m"
                << "Class:" << className << "Section:" << section << "Year:" << yearId;

        return students;

    }

    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students by class:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students by class: " + QString(e.what()));
        return QVector<DataModel::Student>();
    }
    
    return QVector<DataModel::Student>();
}

QVector<DataModel::Student> DataAccess::StudentDataHandler::searchStudentsByStatus(const DataModel::StudentStatus &status)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database isn't open\033[0m " << db.lastError().text();
        Logger::instance().error("Databse isn't open " + db.lastError().text());
        return QVector<DataModel::Student>();
    }
    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT DISTINCT
                e.enrollment_id AS student_id,
                e.enrollment_number AS student_number,
               
                p.person_id,
                p.first_name,
                p.second_name,
                p.third_name,
                p.fourth_name,
                p.gender,
                p.date_of_birth,

                --e.start_date AS enrollment_date,
                --e.end_date AS graduation_date,
                
                se.class_id AS current_class_id,
                se.year_id AS current_year_id,
                e.status AS enrollment_status,
                se.final_average AS current_average,
                se.rank_in_class AS current_rank,
                se.is_eligible_for_exam,
                c.name AS class_name,
                c.section AS class_section,
                c.grade_level,
                ay.name AS year_name,
                es.name AS stage_name
            FROM enrollment e 
            JOIN people USING (person_id)
            JOIN student_enrollment se USING (enrollment_id)
            JOIN classes c USING (class_id)
            JOIN academic_year ay USING(year_id)
            JOIN education_stage es USING (stage_id)
            WHERE e.role = 0
            AND se.status = ?
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
            )");
            query.addBindValue(static_cast<int>(status));

            if(!query.exec())
            {
                qCritical() << "\033[31m Failed to execute get stuednts by status \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by status " + query.lastError().text());
                return QVector<DataModel::Student>(); 
            }
            QVector<DataModel::Student> students;
            while(query.next())
            {
                students.append(createStudentFromQuery(query));
            }
              qInfo() << "\033[32m Found" << students.size() << "students search:\033[0m";
              return students;
            
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students by class:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students by class: " + QString(e.what()));
        return QVector<DataModel::Student>();
    }
    return QVector<DataModel::Student>();
}

QVector<DataModel::Student> DataAccess::StudentDataHandler::getStudentsByType(const DataModel::StudentType &type)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return QVector<DataModel::Student>();
    }

    try
    {

        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
                        SELECT DISTINCT
                e.enrollment_id AS student_id,
                e.enrollment_number AS student_number,
               
                p.person_id,
                p.first_name,
                p.second_name,
                p.third_name,
                p.fourth_name,
                p.gender,
                p.date_of_birth,

                --e.start_date AS enrollment_date,
                --e.end_date AS graduation_date,
                
                se.class_id AS current_class_id,
                se.year_id AS current_year_id,
                e.status AS enrollment_status,
                se.final_average AS current_average,
                se.rank_in_class AS current_rank,
                se.is_eligible_for_exam,
                c.name AS class_name,
                c.section AS class_section,
                c.grade_level,
                ay.name AS year_name,
                es.name AS stage_name
            FROM enrollment e 
            JOIN people USING (person_id)
            JOIN student_enrollment se USING (enrollment_id)
            JOIN classes c USING (class_id)
            JOIN academic_year ay USING(year_id)
            JOIN education_stage es USING (stage_id)
            WHERE e.role = 0
            AND se.status = 1
            AND (
                (:tp = 1 AND se.final_average BETWEEN 50 AND 89)
            OR 
                (:tp = 2 AND se.final_average >= 90)
            OR 
                (:tp = 3 AND se.final_average < 50)
            )

            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
            )");
            query.bindValue(":tp", static_cast<int>(type));

            if(!query.exec())
            {
                qCritical() << "\033[31m Failed to execute get stuednts by tyoe \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by type " + query.lastError().text());
                return QVector<DataModel::Student>(); 
            }

            QVector<DataModel::Student> students;
            while(query.next())
            {
                students.append(createStudentFromQuery(query));
            }
              qInfo() << "\033[32m Found" << students.size() << "students search:\033[0m";
              return students;
            
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students by class:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students by class: " + QString(e.what()));
        return QVector<DataModel::Student>();
    }
    
    return QVector<DataModel::Student>();
}

int DataAccess::StudentDataHandler::getStudentsCount()
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return 0;
    }
    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT COUNT(*)
            FROM enrollment 
            WHERE role = 0
            )");

        if(!query.exec() || !query.next())
        {
                qCritical() << "\033[31m Failed to execute get stuednts by tyoe \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by type " + query.lastError().text());
                return 0; 
        }
        
        return query.value(0).toInt();
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students count:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students count: " + QString(e.what()));
        return 0;
    }
    
    return 0;
}

int DataAccess::StudentDataHandler::getActiveStudentsCount()
{
 const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return 0;
    }
    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT COUNT(*)
            FROM enrollment 
            WHERE role = 0 -- students only
            AND status = 1 -- active status only
            )");

        if(!query.exec() || !query.next())
        {
                qCritical() << "\033[31m Failed to execute get stuednts by tyoe \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by type " + query.lastError().text());
                return 0; 
        }
        
        return query.value(0).toInt();
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students count:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students count: " + QString(e.what()));
        return 0;
    }
    
    return 0;
}

int DataAccess::StudentDataHandler::getInactiveStudentCount()
{
 const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return 0;
    }
    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT COUNT(*)
            FROM enrollment 
            WHERE role = 0 -- students only
            AND status = 0 -- Inactive status only
            )");

        if(!query.exec() || !query.next())
        {
                qCritical() << "\033[31m Failed to execute get stuednts by tyoe \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by type " + query.lastError().text());
                return 0; 
        }
        
        return query.value(0).toInt();
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students count:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students count: " + QString(e.what()));
        return 0;
    }
    
    return 0;
}

int DataAccess::StudentDataHandler::getGraduatedStudentsCount()
{
 const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return 0;
    }
    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT COUNT(*)
            FROM enrollment 
            WHERE role = 0 -- students only
            AND status = 2 -- graduated students only
            )");

        if(!query.exec() || !query.next())
        {
                qCritical() << "\033[31m Failed to execute get stuednts by tyoe \033[0m " << query.lastError().text();
                Logger::instance().error("Failed to execute get students by type " + query.lastError().text());
                return 0; 
        }
        
        return query.value(0).toInt();
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students count:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students count: " + QString(e.what()));
        return 0;
    }
    
    return 0;
}

QVector<QPair<QString, int>> DataAccess::StudentDataHandler::getGraduatedStudentsCountOrderByYear()
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if(!db.isOpen())
    {
        qCritical()<< "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database isn't open " + db.lastError().text());
        return QVector<QPair<QString, int>>();
    }

    try{
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
                SELECT 
                    ay.name AS academic_year_name,
                    COUNT(*) AS total_graduated
                FROM student_enrollment se
                JOIN academic_year ay USING (year_id)
                WHERE se.status = 2
                GROUP BY ay.name
                ORDER BY ay.name;
            )");

            if(!query.exec())
            {
                qCritical() << "\033[31m Failed to execute getGraduatedStudentsCountOrderByYear query \033[0m"<< query.lastError().text();
                Logger::instance().error("Failed to execute getGraduatedStudentsCountOrderByYear query ");
                return QVector<QPair<QString, int>>(); 
            }
            QVector<QPair<QString, int>> graduatedYearPair;

            while(query.next())
            {
                QString yearName = query.value(0).toString();
                int totalGraduated = query.value(1).toInt();
                graduatedYearPair.append(qMakePair(yearName, totalGraduated));
            }

            return graduatedYearPair;


    }
    catch (const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while getting students count:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while getting students count: " + QString(e.what()));
        return QVector<QPair<QString, int>>();
    }

    return QVector<QPair<QString, int>>();
}

// private members
DataModel::Student DataAccess::StudentDataHandler::createStudentFromQuery(const QSqlQuery& query)
{
    DataModel::Student student;
    

    student.set_person_id(query.value("person_id").toInt());
    student.set_student_id(query.value("enrollment_id").toInt());
    student.set_student_enroolment_id(query.value("student_enrollment_id").toInt());
    

    student.set_first_name(query.value("first_name").toString());
    student.set_second_name(query.value("second_name").toString());
    student.set_third_name(query.value("third_name").toString());
    student.set_fourth_name(query.value("fourth_name").toString());

    QString genderStr = query.value("gender").toString();
    student.set_gender(genderStr[0]);
    

    student.set_date_of_birth(query.value("date_of_birth").toDate());
    student.set_enrollment_date(query.value("start_date").toDate());
    student.set_graduation_date(query.value("end_date").toDate());
    

    QVariant studentNumber = query.value("enrollment_number");
    if (!studentNumber.isNull()) 
    {
        student.set_student_number(studentNumber.toString());
    }

    int status = query.value("status").toInt();
    student.set_status(static_cast<DataModel::StudentStatus>(status));
    

    student.set_current_class_id(query.value("class_id").toInt());
    student.set_current_year_id(query.value("year_id").toInt());
    
    QVariant average = query.value("final_average");
    student.set_current_average(average.isNull() ? 0.0 : average.toDouble());
    
    QVariant rank = query.value("rank_in_class");
    student.set_current_rank(rank.isNull() ? 0 : rank.toInt());
    
   // student.set_repeat_count(query.value("repeat_count").toInt());
   // student.set_is_graduated(query.value("is_graduated").toBool());
    //student.set_is_ministerial_exam(query.value("is_ministerial_exam").toBool());
    //student.set_is_eligible_for_exam(query.value("is_eligible_for_exam").toBool());
    

    QVariant className = query.value("class_name");
    if (!className.isNull()) {
        student.set_current_class_name(className.toString());
    }
    
   // student.set_grade_level(query.value("grade_level").toInt());
    
    QVariant yearName = query.value("academic_year_name");
    if (!yearName.isNull()) {
        student.set_current_year_name(yearName.toString());
    }
    
    return student;
}

bool DataAccess::StudentDataHandler::calculateIfShouldRepeat(int lastClassId, int newClassId, double lastAverage, const QSqlDatabase& db)
{
    if (lastClassId == newClassId)
    {
        return true; // Student should repeat the class
    }


    if (lastAverage < 60.0)
    {
        return true; // Student should repeat due to low average
    }

    QSqlQuery getGradesQuery(db);
    getGradesQuery.setForwardOnly(true);
    getGradesQuery.prepare(R"(
        SELECT c.grade_level AS old_grade, nc.grade_level AS new_grade
        FROM classes c
        JOIN classes nc ON 1=1
        WHERE c.class_id = ? AND nc.class_id = ?

    )");
    getGradesQuery.addBindValue(lastClassId);
    getGradesQuery.addBindValue(newClassId);
    if (getGradesQuery.exec() && getGradesQuery.next())
    {
        int oldGrade = getGradesQuery.value("old_grade").toInt();
        int newGrade = getGradesQuery.value("new_grade").toInt();

        if (newGrade <= oldGrade)
        {
            return true; // Student should repeat due to same or lower grade level
        }
    }

    return false;
}

bool DataAccess::StudentDataHandler::calculateRankForStudentInClass(int classId, int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for calculating student rank");
        return false;
    }

    try{
        QSqlQuery getStudenetsQuery(db);
        getStudenetsQuery.setForwardOnly(true);
        getStudenetsQuery.prepare(R"(
            SELECT se.student_enrollment_id, se.final_average
            FROM student_enrollment se
            WHERE se.class_id = ?
            AND se.year_id = ?  
            AND se.status = 1 -- Active status only
            AND se.final_average IS NOT NULL
            ORDER BY se.final_average DESC
        )");
        getStudenetsQuery.addBindValue(classId);
        getStudenetsQuery.addBindValue(yearId);

        if(!getStudenetsQuery.exec())
        {
            qCritical() << "\033[31m Failed to retrieve students for class ID: \033[0m" << classId;
            Logger::instance().error("Failed to retrieve students for class ID: " + QString::number(classId));
            return false;
        }
        QVector<QPair<int, double>> studentAverages; // int for student id, double for average
        while(getStudenetsQuery.next())
        {
            int studentEnrollmentId = getStudenetsQuery.value("student_enrollment_id").toInt();
            double average = getStudenetsQuery.value("final_average").toDouble();
            studentAverages.append(qMakePair(studentEnrollmentId, average));
        }

        if(studentAverages.isEmpty())
        {
            qInfo() << "\033[33m No students found for class ID: \033[0m" << classId;
            Logger::instance().info("No students found for class ID: " + QString::number(classId));
            return true; // No students to rank
        }

        
        if(studentAverages.isEmpty()) return true;


        std::sort(studentAverages.begin(), studentAverages.end(), [](const QPair<int, double>& a, const QPair<int, double>& b)
        {
            return a.second > b.second; // Descending order
            // { (1, 78.5), (2, 91.0), (3, 85.3) }
            // after sort { (2, 91.0), (3, 85.3), (1, 78.5) }
        });


        int currentRank = 1;
        int nextRank = 1;
        double previousAverage = -1.0;

        QVector<QPair<int, int>> rankUpdates; // student id, rank
        for (const auto& pair : studentAverages)
        {
            int studentId = pair.first;
            double average = pair.second;

            if (std::abs(average - previousAverage) > 0.001) // New average, update rank
                currentRank = nextRank;

            rankUpdates.append(qMakePair(studentId, currentRank));
            previousAverage = average;
            nextRank++;
        }

        if(!db.transaction())
        {
            qCritical() << "\033[31m Failed to start transaction for updating ranks in class ID: \033[0m" << classId;
            Logger::instance().error("Failed to start transaction for updating ranks in class ID: " + QString::number(classId));
            return false;
        }

        QSqlQuery updateRankQuery(db);
        updateRankQuery.prepare("UPDATE student_enrollment SET rank_in_class = ? WHERE student_enrollment_id = ?");



  
        for(const auto& [id, r] : rankUpdates)
        {
            updateRankQuery.bindValue(0, r); // rank
            updateRankQuery.bindValue(1, id);  // student id
            if(!updateRankQuery.exec())
            {
                qCritical() << "\033[31m Failed to update rank for student enrollment ID: \033[0m" << id;
            }
        }

        if(!db.commit())
        {
            qCritical() << "\033[31m Failed to commit transaction for updating ranks in class ID: \033[0m" << classId;
            Logger::instance().error("Failed to commit transaction for updating ranks in class ID: " + QString::number(classId));
            db.rollback();
            return false;
        }

        qInfo() << "\033[32m Successfully calculated and updated ranks for class ID:\033[0m" << classId;
        return true;
}catch (const std::exception& e)
{
    qCritical() <<"\033[31mException occurred while calculating student ranks: " <<e.what();
    Logger::instance().error("Exception occurred while calculating student ranks: " + QString(e.what()));

}

    return false;
}



bool DataAccess::StudentDataHandler::isStudentExists(int studentId, const QSqlDatabase& db)
{

    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for checking student existence");
        return false;
    }
    try{
    QSqlQuery chkQuery(db);
    chkQuery.setForwardOnly(true);
    chkQuery.prepare(R"(
        SELECT 1 FROM enrollment
        WHERE enrollment_id = ? AND role = 0
    )");
    chkQuery.addBindValue(studentId);
    if(chkQuery.exec() && chkQuery.next())
    {
        return true;
    }
    else
    {
        qInfo() << "\033[33m Student does not exist with ID: \033[0m" << studentId;
        Logger::instance().info("Student does not exist with ID: " + QString::number(studentId));
    }
}catch (const std::exception& e)
{
    qCritical() <<"\033[31mException occurred while checking student existence: " <<e.what();
    Logger::instance().error("Exception occurred while checking student existence: " + QString(e.what()));
}
    return false;
}

bool DataAccess::StudentDataHandler::isStudentExists(const QString &studentNumber)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    
    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database isn't open " << db.lastError().text();
        Logger::instance().error("Database is not open " + db.lastError().text());
        return false;
    }

    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT EXISTS(
                SELECT 1 FROM enrollment
                WHERE student_number = ?
                LIMIT 1
        )
            )");
            query.addBindValue(studentNumber);


            if (!query.exec() || !query.next())
            {
                qCritical() << "\033[31m Failed to execute \033[0m"<< query.lastError().text();
                Logger::instance().error("Failed to execute " + query.lastError().text());
                return false; 
            }

            return query.value(0).toBool();
            
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool DataAccess::StudentDataHandler::isStudentEnrolled(const QString &studentNumber, int yearId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
        if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m" << db.lastError().text();
        Logger::instance().error("Database connection is not open ");
        return false;
    }

    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT EXISTS(
                SELECT 1 
                    FROM student_enrollment se
                    JOIN enrollment e USING (enrollment_id)
                    WHERE e.enrollment_number = ?
                    AND se.year_id = ?
                    AND se.status = 1
                    AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
            ) AS is_enrolled
            )");
        query.addBindValue(studentNumber);
        query.addBindValue(yearId);
        if (!query.exec()) 
        {
            qCritical() << "\033[31m Failed to check student enrollment:\033[0m" << query.lastError().text();
            Logger::instance().error("Failed to check student enrollment: " + query.lastError().text());
            return false;
        }

        if (query.next()) 
        {
            bool isEnrolled = query.value("is_enrolled").toBool();
            
            qDebug() << "Student" << studentNumber << "enrolled in year" << yearId << ":" << isEnrolled;
            return isEnrolled;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return false;
}

bool DataAccess::StudentDataHandler::isStudentActive(int studentId, const QSqlDatabase& db)
{
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for checking student activity");
        return false;
    }

    try{
    QSqlQuery chkQuery(db);
    chkQuery.setForwardOnly(true);
    chkQuery.prepare(R"(
        SELECT EXISTS(
            SELECT 1 FROM enrollment
            WHERE enrollment_id = ? AND role = 0 AND status = 1 -- Active status
            LIMIT 1
    )
)");

    chkQuery.addBindValue(studentId);
    if(chkQuery.exec() && chkQuery.next())
    {
        // the student is active
        qInfo() << "\033[32m Student is active with ID: \033[0m" << studentId;
        return chkQuery.value(0).toBool();
    } 
}
catch (const std::exception& e)
{
    qCritical() <<"\033[31mException occurred while checking student activity: " << e.what();
    Logger::instance().error("Exception occurred while checking student activity: " + QString(e.what()));
     return false;
}   
 return false;   
}

bool DataAccess::StudentDataHandler::isStudentGraduated(int studentId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();
    if (!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for checking student activity");
        return false;
    }

    try
    {
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECT EXISTS(
                SELECT 1 FROM student_enrollment
                WHERE status = 2
                AND student_enrollment_id = ? 
            )
            )");
        if(!query.exec())
        {
            qCritical() << "\033[31mFailed to execute query:\033[0m" << query.lastError().text();
            Logger::instance().error("Failed to execute query: " + query.lastError().text());
            return false;
        }

        if(!query.next())
        {
            qCritical() << "\033[31mNo result returned for student enrollment check\033[0m";
            return false;
        }

        return query.value(0).toBool();

            
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return false;
}

 

bool DataAccess::StudentDataHandler::canGraduateStudent(int studentId)
{
    const auto& connWrapper = DatabaseManager::instance().getConnection();
    QSqlDatabase db = connWrapper->database();

        if (!db.isOpen()) 
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open");
        return false;
    }
    if(!db.transaction())
    {
        qCritical() << "\033[31m Failed to start transaction. \033[0m" << db.lastError().text();
        Logger::instance().error("Failed to start transaction. " + db.lastError().text());
        return false;
    }

    try
    {
        if(!isStudentActive(studentId, db))
        {
            qWarning() <<"\033[33m Student not active\033[0m";
            return false;
        }

        QSqlQuery currentClassQuery(db);
        currentClassQuery.setForwardOnly(true);
        currentClassQuery.prepare(R"(
            SELECT se.class_id, se.year_id, c.grade_level, es.max_grade
            FROM student_enrollment se
            JOIN classes c ON se.class_id = c.class_id
            JOIN education_stages es ON c.stage_id = es.stage_id 
            WHERE se.enrollment_id = ?
            AND se.status = 1
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
            LIMIT 1
        )");
        currentClassQuery.addBindValue(studentId);

        if(!currentClassQuery.exec() || !currentClassQuery.next())
        {
            qWarning() <<"\033[33m Could not find current class for student.\033[0m \033[31m\n OR ERROR: " <<currentClassQuery.lastError().text();           
            if(currentClassQuery.lastError().text() != "")
            Logger::instance().error(currentClassQuery.lastError().text());
            return false;
        }

        int classId = currentClassQuery.value("class_id").toInt();
        int yearId = currentClassQuery.value("year_id").toInt();
        int currentGrade = currentClassQuery.value("grade_level").toInt();
        //int maxGrade= currentClassQuery.value("max_grade").toInt(); //TODO: I will add it later 
        if(!isFinalGrade(currentGrade))
        {
            qWarning() << "\033[33m Studnet is not in final grage level. Current grade is: \033[0m" << currentGrade;
            return false;
        }

        if(!evaluateAllSubjectsAbove50(studentId, classId, yearId, db)) 
        {
            qWarning() << "\033[33m Student failed in some subjects\033[0m";
            return false;
        }

        //TODO: cheak the total absences here

        
          qInfo() << "\033[32m Student" << studentId << "is eligible for graduation\033[0m";
          return true;
    }
    catch(const std::exception& e)
    {
        qCritical() << "\033[31m Exception occurred while checking graduation eligibility:\033[0m" << e.what();
        Logger::instance().error("Exception occurred while checking graduation eligibility: " + QString(e.what()));
        return false;
    }
}
// // TODO: complete this function ...
// bool DataAccess::StudentDataHandler::shouldStudentPass(int studentId, int classId, int yearId, double passMark, const QSqlDatabase& db)
// {
//     QSqlQuery getStudentInfo(db);
//     getStudentInfo.prepare(R"(
//             SELECT             
//         )");
//     return false;
// }



bool DataAccess::StudentDataHandler::evaluateAllSubjectsAbove50(int studentId, int classId, int yearId, const QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.prepare(R"(
        SELECT 
            s.subject_id,
            s.name AS subject_name,
            (MAX(CASE WHEN g.exam_type = 4 THEN g.score ELSE NULL END) * 100.0 /
             NULLIF(MAX(CASE WHEN g.exam_type = 4 THEN g.max_score ELSE NULL END), 0)) AS final_percentage
            
            FROM subjects s
            JOIN curriculum cu ON s.subject_id = cu.subject_id
            JOIN course_offering co ON cu.curriculum_id = co.curriculum_id 
            JOIN student_enrollment se ON se.class_id = cu.class_id
            AND se.year_id = cu.year_id
            LEFT JOIN grades g ON g.offering_id = co.offering_id
            AND g.student_enrollment_id = se.student_enrollment_id
            WHERE se.enrollment_id = ?
            AND se.class_id = ? 
            AND se.year_id = ? 
            GROUP BY s.subject_id, s.name
        )");
        query.addBindValue(studentId);
        query.addBindValue(classId);
        query.addBindValue(yearId);
        
        if(!query.exec())
        {
            qFatal() << "\033[31m Failed to evaluate subjects: \033[0m" << query.lastError().text();
            Logger::instance().error("Failed to evaluate subjects: " + query.lastError().text());
            return false;
        }

    bool allSubjectsPass = true;
    std::vector<std::string> passedSubjects;
    std::vector<std::string> failedSubjects;

    while (query.next()) 
    {
        std::string subjectName = query.value("subject_name").toString().toStdString();
        double finalPercentage = query.value("final_percentage").toDouble();
        
        if(finalPercentage  >= 50.0)
        {
            passedSubjects.push_back(subjectName + " (" + std::to_string(finalPercentage) + ")");
        }
        else
        {
            allSubjectsPass = false;
            failedSubjects.push_back(subjectName  + " (" + std::to_string(finalPercentage) + ")");
            std::cerr << "\033[31m Failed subject: " << subjectName
                      << " score: " << std::fixed << std::setprecision(1)
                      <<finalPercentage <<"%\033[0m" <<std::endl;
        }
    }
    return allSubjectsPass;
}
// TODO: compelete this function tmrw

DataModel::StudentEnrollment DataAccess::StudentDataHandler::getStudentEnrollmentById(int enrollmentId)
{

    const auto& connWrapper = DatabaseManager::instance().getConnection();
    const QSqlDatabase& db = connWrapper->database();

    if(!db.isOpen())
    {
        qCritical() << "\033[31m Database connection is not open\033[0m";
        Logger::instance().error("Database connection is not open for getting student by ID");
        return DataModel::Student();
    }
    try{
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare(R"(
            SELECTC 
            e.enrollment_id,
            e.person_id,
            
            p.first_name,
            p.second_name,
            p.third_name,
            p.fourth_name,
            p.gender,
            p.date_of_birth,
            
            e.enrollment_number AS student_number,
            e.start_date AS enrollment_date,
            e.end_date AS graduation_date,
            e.status AS enrollment_status,
            
            se.class_id AS current_class_id,
            se.year_id AS current_year_id,
            se.final_average AS current_average,
            se.rank_in_calss AS current_rank,
            se.repeat_count 
            se.is_graduated,
            se.is_repeated,
            se.is_ministerial_exam,
            se.is_eligible_for_exam,

            c.name AS class_name,
            c.section,
            c.grade_level,
            ay.name AS year_name
        FROM enrollment e
        LEFT JOIN people p ON e.person_id = p.peron_id
        LEFT JOUN student_enrollment se ON e.enrollment_id = se.enrollment_id 
            AND (se.end_date IS NULL OR se.end_date > CURRENT_DATE)
        LEFT JOIN classes c ON se.class_id = c.class_id
        LEFT JOIN academic_years ay ON se.year_id = ay.year_id
        WHERE se.enrollment_id = ? AND e.role = 0
        ORDER BY se.start_date DESC
        LIMIT 1 
            )");

            query.addBindValue(enrollmentId);
            if(!query.exec() || !query.next())
            {
                qCritical() << "\033[31m Student not found with ID: \033[0m" << studentId;
                Logger::instance().error("Student not found with ID: " + QString::number(studentId));
                return DataModel::Student();
            }
            DataModel::StudentEnrollment student = createStudentFromQuery(query);
            return student;
        }
        catch(const std::exception& e)
        {
            qCritical() << "Error: " <<e.what() << "\n";
            return DataModel::StudentEnrollment();
        }

    return DataModel::StudentEnrollment();
}

bool DataAccess::StudentDataHandler::isFinalGrade(int Grade)
{
    return (Grade == 6 || 9 || 12) ?  true : false;
}