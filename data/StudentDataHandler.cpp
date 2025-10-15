#include "StudentDataHandler.h"
#include"DatabaseManager/databaseManager.h"
#include"Logger.h"
#include"PeopleDataHandler.h"

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
        if(isStudentExists(student.student_number))
        {
            qCritical() << "\033[31m Student already exists with number:\033[0m" << student.student_number;
            Logger::instance().error("Student already exists with number: " + student.student_number);
            db.rollback();
            return std::nullopt;
        }

        int personId = student.person_id;
        if(personId == 0)
        {
            DataModel::Person person;
            person.first_name =student.first_name;
            person.second_name = student.second_name;
            person.third_name = student.third_name;
            person.fourth_name = student.fourth_name;
            person.gender = student.gender;
            person.date_of_birth = student.date_of_birth;

            auto newPersonId = PeopleDataHandler::addPerson(person);
            if (!newPersonId.has_value()) {
                qCritical() << "\033[31m Failed to add person record\033[0m";
                db.rollback();
                return std::nullopt;
            }
            personId = newPersonId.value();
        }
        else
        {
            if (!PeopleDataHandler::isPersonExist(personId))
            {
                qCritical() << "\033[31m Person not found with ID:\033[0m" << personId;
                db.rollback();
                return std::nullopt;
            }
        }

// I will complete it tmrw :]

}
