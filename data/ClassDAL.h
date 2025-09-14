#ifndef CLASSDAL_H
#define CLASSDAL_H
#include<QString>
#include<optional>
#include"DataModels.h"
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
    static QVector<DataModel::Class> getAllClasses(int limit = 100, int offset =0);




};

#endif // CLASSDAL_H
