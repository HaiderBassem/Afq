#pragma once

#include <QString>
#include <QChar>
#include <QDate>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"
namespace DataModel
{

class Guardian : public BaseEntity
{
public:
    int guardian_id = 0;
    int person_id = 0;
    QString occupation;
    QString work_phone;
    QString education_level;


    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    QString full_name; 
    QChar gender;
    QDate date_of_birth;
    QString relationship;


    QString studentFirstName;
    QString studentSecondName;
    QString studentThirdName;
    QString studentFourthName;
    QString studentFullName;

    bool isPrimary = false;
    QString className;


    QString guardian_name;


    Guardian() = default;


    QString getFullName() const;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}