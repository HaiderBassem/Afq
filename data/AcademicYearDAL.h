#ifndef ACADEMICYEARDAL_H
#define ACADEMICYEARDAL_H
#include<optional>
#include<QString>
#include<QDate>
#include<QVector>
#include"DataModels.h"

namespace DataAccess {


class AcademicYearDAL
{
public:
    //CRUD
    static std::optional<int> createAcademicYear(const QString& name, const QDate& start_date, const QDate& end_date, bool isActive = false);
    static std::optional<DataModel::AcademicYear> getAcademicYearById(int id);
    static bool uodateAcademicYear(int id, const QString& name, const QDate& start_date, const QDate& end_date);
    static bool deleteAcademicYear(int id);

//Queries
    static QVector<DataModel::AcademicYear> getAllAcademicYears();
    static std::optional<DataModel::AcademicYear> getCurrentAcademicYear();
    static std::optional<DataModel::AcademicYear> getAcademicYearByDate(const QDate& start_date, const QDate& end_date);
    static std::optional<DataModel::AcademicYear> getUpcomingAcademicYear();
    static std::optional<DataModel::AcademicYear> getPreviousAcademicYear();
//validation
    static bool isYearActive(int id);

//statistics
    static double getYearProgress(int yearId);
    static int getRemainingDays(int yearId);

};
}//namespace DataAccess
#endif // ACADEMICYEARDAL_H
