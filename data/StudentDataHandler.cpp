#include "StudentDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"
#include"PeopleDataHandler.h"
#include"GuardianDataHandler.h"
#include"DataModel/Student.h"

#include<QSqlQuery>
#include<QSqlError>
#include<QSqlDatabase>

#include<optional>


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
            SELECTC 
            e.enrollment_id AS student_id,
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
        WHERE e.enrollment_id = ? AND e.role = 0
        ORDER BY se.start_date DESC
        LIMIT 1 
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
    
    return DataModel::Student();

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
    query.setForwardOnly(true); 
    

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















// private members
DataModel::Student DataAccess::StudentDataHandler::createStudentFromQuery(const QSqlQuery& query)
{
    DataModel::Student student;
    

    student.set_person_id(query.value("person_id").toInt());
    student.set_student_id(query.value("enrollment_id").toInt());
    

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
    
    student.set_repeat_count(query.value("repeat_count").toInt());
    student.set_is_graduated(query.value("is_graduated").toBool());
    student.set_is_ministerial_exam(query.value("is_ministerial_exam").toBool());
    student.set_is_eligible_for_exam(query.value("is_eligible_for_exam").toBool());
    

    QVariant className = query.value("class_name");
    if (!className.isNull()) {
        student.set_current_class_name(className.toString());
    }
    
    student.set_grade_level(query.value("grade_level").toInt());
    
    QVariant yearName = query.value("academic_year_name");
    if (!yearName.isNull()) {
        student.set_current_year_name(yearName.toString());
    }
    
    return student;
}