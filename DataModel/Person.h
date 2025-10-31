#pragma once

#include <QString>
#include <QStringList>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include"DataModel/BaseEntity.h"
#include"DataModel/Person.h"


namespace DataModel
{

enum class Gender : char
{
    Male = 'M',
    Female = 'F'
};

inline Gender charToGender(char c)
{
    return (c == 'F' || c == 'f') ? Gender::Female : Gender::Male;
}

inline QString genderToString(Gender g)
{
    switch (g)
    {
    case Gender::Male: return QObject::tr("Male");
    case Gender::Female: return QObject::tr("Female");
    default: return QObject::tr("Male");
    }
}

inline Gender charToGender(QChar c)
{
    if (c == 'M' || c == 'm') return Gender::Male;
    if (c == 'F' || c == 'f') return Gender::Female;
    return Gender::Male;
}


class Person : public BaseEntity
{
public:
    int person_id = 0;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    Gender gender = Gender::Male;
    QDate date_of_birth;
    QDateTime created_at;
    QDateTime updated_at;


    QString getFullName() const;
    int getAge() const;


    
    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}