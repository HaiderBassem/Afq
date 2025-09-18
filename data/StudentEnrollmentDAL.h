#ifndef STUDENTENROLLMENTDAL_H
#define STUDENTENROLLMENTDAL_H

#include "DataModels.h"
#include<QVector>
#include<QDate>
#include<QString>
#include<optional>
namespace DataAccess {


class StudentEnrollmentDAL
{
public:

    static std::optional<int> enrollStudentInClass(int enrollmentId, int yearId, const QDate& start_date);
    static std::optional<DataModel::StudentEnrollment> getStudentEnrollmentById(int studentEnrollmentId);
    static bool updatestudentEnrollment(int studentEnrollmentId, int classId);
    static bool deleteStudentEnrollment(int StudentEnrollmentId);

    static QVector<DataModel::StudentEnrollment> getStudentsByClass(int classId, int yearId);
    static QVector<DataModel::StudentEnrollment> getStudentsEnrollmentHistory(int enrollmentId);
    static std::optional<DataModel::StudentEnrollment> getStudentCurrentClass(int enrollmentId, int yearId);
    static QVector<DataModel::StudentEnrollment> getAllStudentsEnrollments(int limit = 100, int offset = 0);

    static bool transferStudentToClass(int studentEnrollmentId, int newClassId);
    static bool promoteStudent(int StudentEnrollmentId, int newClassId, int newYearId);
    static bool graduateStudent(int StudentEnrollmentId);
    static bool withdrawStudent(int enrollmentId, const QDate& end_date);

    static bool isStudentInClass(int enrollmentId);
    static bool canEnrollInClass(int enrollmentId, int classId, int yearId);

    static QVector<DataModel::StudentEnrollment> getStudentsForPromothion(int classId, int yearId);
    static QVector<DataModel::StudentEnrollment> getDropOutCandidate(int yearId, double minAttendanceRate = 0.75);
    static bool blukPromoteClass(int classId, int fromYearId, int toYearId, int newClassId);
    static QVector<int> getClassmates(int StudentEnrollmentId);
    static int getTotalCount();

};
}
#endif // STUDENTENROLLMENTDAL_H
