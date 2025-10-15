#ifndef STUDENTDATAHANDLER_H
#define STUDENTDATAHANDLER_H

#include"DataModel/Student.h"
#include<QString>
#include<QVector>
#include<QMap>
#include<QSqlQuery>
#include<optional>
namespace DataAccess {


class StudentDataHandler
{
public:

    static std::optional<int> addStudent(const DataModel::Student& student);
    static bool updateStudent(int studentId, const DataModel::Student& student);
    static bool deleteStudent(int studentId);
    static DataModel::Student getStudentById(int studentId);
    static DataModel::Student getStudentByPersonId(int personId);
    static DataModel::Student getStudentByStudentNumber(int studentNumber);
    static QVector<DataModel::Student> getAllStudents();
    static QVector<DataModel::Student> getActiveStudents();
    static QVector<DataModel::Student> getInactiveStudent();
    static QVector<DataModel::Student> getGraduatedStudents();
    static QVector<DataModel::Student> getGraduatedStudents(int yearId);
    static bool reenrollStudent(int StudentId, int ClassId, int yearId);
    static bool updateStudentAcademicInfo(int studentId, double average, int rank, int absences);

    static int enrollStudent(int studentId, int classId, const QString& section, int yearId);
    static bool withdrawStudent(int studentId, const QString& reason = "");
    static bool transferStudent(int studentId, int newClassId, const QString& section, const QString& reason = "");

    //searching

    static QVector<DataModel::Student> searchStudentsByName(const QString& fNmae, const QString& sName ="", const QString& tName = "",const QString& ftName ="", std::optional<int> classId= std::nullopt, const QString& section="",std::optional<int> yearId = std::nullopt);
    static DataModel::Student searchStudentByStudentNumber(const QString& studentNumber);
    static QVector<DataModel::Student> getStdentsByClass(const QString& className,const QString& section ,int yearId);
    static QVector<DataModel::Student> getStdentsByClass(const QString& className, int yearId);
    static QVector<DataModel::Student> searchStudentsByStatus(const DataModel::StudentStatus& status);
    static QVector<DataModel::Student> getStudentsByType(const DataModel::StudentType& type);


    // reports

    static int getStudentsCount();
    static int getActiveStudentsCount();
    static int getInactiveStudentCount();
    static int getGraduatedStudentsCount();
    static int getStudentsCountByClass(int classId, int yearId);
    static int getStudentsCountByStatus(DataModel::StudentStatus status);
    static int getStudentsCountByAcademicYear(int yearId);
    static int getStudentsCountByType(DataModel::StudentType type);
    static QMap<DataModel::StudentStatus, int> getStudentsCountByStatus();
    static QMap<DataModel::StudentType, int> getStudentsCountByType();

    // checking

    static bool isStudentExists(int studentId);
    static bool isStudentExistsByPersonId(int personId);
    static bool isStudentExists(const QString& studentNumber);
    static bool isStudentEnrolled(const QString& studentNumber, int yearId);
    static bool isStudentActive(int studentId);
    static bool isStudentGraduated(int studentId);
    static bool canGraduateStudent(int studentId);
    static bool isStudentPassedAllSubjects(int studentId);


    static DataModel::StudentEnrollment getCurrentEnrollment(int studentId);
    static DataModel::StudentEnrollment getStudentEnrollmentById(int enrollmentId);




    // students manipulation
    static bool promoteClassStudents(int classId, int newClassId);
    static bool bulkUpdateStudentsStatus(const QVector<int>& studentIds, DataModel::StudentStatus status);
    static bool transferClassStudents(int fromClassId, int toClassId);
    static QVector<DataModel::Student> getStudentsEligibleForGraduation(int classId);
    static QVector<DataModel::Student> getStudentsAtRisk(int classId, double threshold = 50.0);
    static QVector<DataModel::Student> getTopPerformingStudents(int classId, int limit = 10);
    static int getFailedStudentsCount(int classId, double passMark = 50.0);
    static QVector<DataModel::Student> getFailedStudents(int classId, double passMark = 50.0);
    static bool promoteOrRepeatClass(int classId, int newClassId, double passMark = 50.0);

    // student summary
    static DataModel::StudentSummary getStudentSummary(int studentId);
    static QVector<DataModel::StudentSummary> getClassSummary(int classId);
    static DataModel::StudentSummary getStudentProgressReport(int studentId);


    // managemnet
    static QString generateStudentNumber(int yearId);

    static bool backupStudentRecords();
    static bool restoreStudentRecords(const QString& backupFile);
    static bool archiveStudentRecord(int studentId);
    static bool restoreStudentRecord(int studentId);





private:
    StudentDataHandler() = delete;
    ~StudentDataHandler() = delete;

    static DataModel::Student createStudentFromQuery(const QSqlQuery& query);
    static DataModel::StudentSummary createStudentSummaryFromQuery(const QSqlQuery& query);
    static DataModel::StudentEnrollment createStudentEnrollmentFromQuery(const QSqlQuery& query);

};
}
#endif // STUDENTDATAHANDLER_H
