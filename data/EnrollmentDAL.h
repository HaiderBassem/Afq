#ifndef ENROLLMENTDAL_H
#define ENROLLMENTDAL_H

#include"DataModels.h"

#include<optional>
#include<QString>
#include<QVector>
#include<QDate>
#include<QMap>

namespace DataAccess
{
class EnrollmentDAL
{
    public:

    //CRUD OPs

    static std::optional<int> createEnrollment(int personId, int schoolId, const QString& role, const QDate& start_date);
    static std::optional<DataModel::Enrollment> getEnrollment(int enrolment_id);
    static bool updateEnrollment(int enrollment_id, const QString& role, const QDate& end_date);
    static bool deleteEnrollment(int enrollment_id);

    static QVector<DataModel::Enrollment> getEnrollmentByPersonId(int personId);
    static QVector<DataModel::Enrollment> getEnrollmentBySchoolId(int schoolId, const QString& role = "");
    static QVector<DataModel::Enrollment> getActiveEnrollment(int schoolId);
    static std::optional<QVector<DataModel::Enrollment>> getPersonEnrollment(int personId, int schoolId);
    static QVector<DataModel::Enrollment> getAllEnrollment(int limit = 100, int offset = 0);

    static bool isPersonEnrolled(int personId,int schoolId);
    static bool hasActiveEnrollmet(int personId, int schoolId, const QDate& end_date);

    static int getActiveStudentsCount(int enrollment_id);
    static int getActiveTeachersCount(int schoolId);
    static int getActiveStaffCount(int schoolId);
    static QMap<QString, int> getRoleDistribution(int schoolId);


    static bool transferPersonToSchool(int personId, int fromSchoolId, int toSchoolId);
    static QVector<DataModel::Enrollment> getEnrollmentHistory(int personId);
    static QVector<DataModel::Enrollment> getRecentEnrollments(int schoolId, int days = 30);
    static int getTotalCount();

    };
}

#endif // ENROLLMENTDAL_H
