#pragma once

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
class Curriculum : public BaseEntity
{
public:
    int curriculum_id = 0;
    int class_id = 0;
    int subject_id = 0;
    int year_id = 0;
    QString textbook_name;

    QString class_name;
    QString subject_name;
    QString year_name;

    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}