#ifndef CLASSDAL_H
#define CLASSDAL_H
#include"DataModels.h"
#include<optional>
#include<QString>

namespace DataAccess
{

class ClassDAL
{
public:
    static std::optional<int> createClass(int schoolId, const QString& name, const QString& section, int year);
    static std::optional<DataModel::Class> getClassById(int classId);
    static bool updateClass(int classId, const QString& name, const QString& section, int year);
    static bool deleteClass(int classId);

    static QVector<DataModel::Class> getClassesBySchool(int schoolId);
    static QVector<DataModel::Class> getClassesByYear(int year);
    static QVector<DataModel::Class> searchClasses(const QString& searchTerm);
    static QVector<DataModel::Class> getAllClasses(int limit =100, int offset = 0);
    //static QString getClassFullName(int classId);

    static int studentCount(int classId, int year);
    static int subjectCount(int classId, int year);
    static QVector<DataModel::ClassSummary> getclassSummary(int classId, int year);
    static double getClassAttendanceRate(int classId, int yearId);

    //static int getTotalCount();

};

}

#endif // CLASSDAL_H
