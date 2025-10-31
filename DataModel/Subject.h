#pragma once

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
class Subject : public BaseEntity
{
public:
    int subject_id = 0;
    QString name;
    QString code;
    double passing_grade = 50.0;
    int stage_id = 0;

    QString stage_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}
