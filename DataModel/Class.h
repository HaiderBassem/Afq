#pragma once

#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
class Class : public BaseEntity
{
public:
    int class_id = 0;
    int stage_id = 0;
    int grade_level = 1;
    QString name;
    QString section;
    int year = 0;
    bool has_ministerial_exam = false;

    // ---- Additional fields for joins ----
    QString stage_name;

    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;

    QString getFullClassName() const;
};

}