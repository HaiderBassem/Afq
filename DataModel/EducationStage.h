#pragma once

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"
namespace DataModel
{
class EducationStage : public BaseEntity
{
public:
    int stage_id = 0;
    QString name;
    int min_grade = 0;
    int max_grade = 0;
    bool has_ministerial_exam = false;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;


    bool containsGrade(int grade) const;
};
}