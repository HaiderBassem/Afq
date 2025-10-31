#pragma once

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"
namespace DataModel
{
class CourseOffering : public BaseEntity
{
public:
    int offering_id = 0;
    int curriculum_id = 0;
    int teacher_enrollment_id = 0;
    int term_id = 0;
    int current_students = 0;

    QString subject_name;
    QString teacher_name;
    QString class_name;
    QString term_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}