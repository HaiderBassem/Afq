#pragma once

#include <QString>
#include <QDate>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"
namespace DataModel
{
class AcademicYear : public BaseEntity
{
public:
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    int holidays_count = 0;

    // ---- BaseEntity overrides ----
    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;

    bool isCurrentYear() const;
    int getDurationInDays() const;
};
}