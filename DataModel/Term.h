#pragma once

#include <QString>
#include <QDate>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"
namespace DataModel
{
class Term : public BaseEntity
{
public:
    int term_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    QDate exam_start_date;
    QDate exam_end_date;


    QString year_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
    bool isCurrentTerm() const;
    int getWeeksCount() const;
};
}