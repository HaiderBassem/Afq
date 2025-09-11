#ifndef SCHOOLDAL_H
#define SCHOOLDAL_H

#include <optional>
#include <QString>
#include <QVector>
#include <QPair>
#include "dataModels.h"

namespace DataAccess {

class SchoolDAL
{
public:
    // Basic CRUD Operations
    static std::optional<int> addSchool(const QString& name);
    static bool updateSchool(const int& id, const QString& name);
    static bool deleteSchool(const int& id);
    static std::optional<DataModel::School> getSchoolById(const int& id);
    static QVector<DataModel::School> getSchoolByName(const QString& name);


    static bool isSchoolExist(const int& id);
    static bool isSchoolExist(const QString& name);


    static QVector<DataModel::School> getAllSchools();
    static std::optional<int> getSchoolCount();
    static QVector<DataModel::School> getSchoolsPaginated(int offset, int limit);

    // Searching
    static QVector<DataModel::School> searchSchoolsByPartialName(const QString& partialName);
    //static QVector<DataModel::School> getSchoolsInRange(int startId, int endId);

    // Batch operations

    static bool deleteMultipleSchools(const QVector<int>& ids);




    // Statistics and reporting

    static int getActiveStudentsCount(int schoolID);
    static int getActiveTeacherCount(int schoolID);
    static int getClassesCount(int schoolID, int year);



};

} // namespace DataAccess

#endif // SCHOOLDAL_H
