#ifndef CLASSSUMMARY_H
#define CLASSSUMMARY_H

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
    


class ClassSummary : public BaseEntity
{
public:
    int class_id = 0;
    QString class_name;
    QString section;
    int total_students = 0;
    int active_students = 0;
    double class_average = 0.0;
    int subjects_count = 0;
    QString teacher_name;

    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
} // namespace DataModel
#endif // CLASSSUMMARY_H
