#ifndef STUDENTDATAHANDLER_H
#define STUDENTDATAHANDLER_H

#include"DataModel.h"
#include<QString>
#include<QVector>
#include<QMap>

#include<optional>
namespace DataAccess {


class StudentDataHandler
{
public:

    static std::optional<int> addStudent(const DataModel::Person& student);
    static bool updateStudent(int studentPersonId, const DataModel::Person& student);
    static bool deleteStudent(int studentPersonId);
    static DataModel::Person
    static QVector<DataModel::Person> getAllStudents();


private:
    StudentDataHandler() = delete;
    ~StudentDataHandler() = delete;
};
}
#endif // STUDENTDATAHANDLER_H
