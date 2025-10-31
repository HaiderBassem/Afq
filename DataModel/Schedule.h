#pragma once

#include <QString>
#include <QJsonObject>
#include <QTime>
#include "DataModel/BaseEntity.h"
//#include "DayOfWeek.h" // enum DayOfWeek و intToDayOfWeek / dayOfWeekToString 


namespace DataModel
{


enum class DayOfWeek
{
    Sunday = 1,
    Monday = 2,
    Tuesday = 3,
    Wednesday = 4,
    Thursday = 5,
    Unknown = -1
};

inline DayOfWeek intToDayOfWeek(int i)
{
    if (i >= 1 && i <= 5) return static_cast<DayOfWeek>(i);
    return DayOfWeek::Unknown;
}

inline QString dayOfWeekToString(DayOfWeek d)
{
    switch (d) 
    {
    case DayOfWeek::Sunday: return QObject::tr("Sunday");
    case DayOfWeek::Monday: return QObject::tr("Monday");
    case DayOfWeek::Tuesday: return QObject::tr("Tuesday");
    case DayOfWeek::Wednesday: return QObject::tr("Wednesday");
    case DayOfWeek::Thursday: return QObject::tr("Thursday");
    default: return QObject::tr("Unknown");
    }
}





class Schedule : public BaseEntity
{
public:
    int schedule_id = 0;
    int offering_id = 0;
    DayOfWeek day_of_week = DayOfWeek::Sunday;
    int period_number = 1;
    QTime start_time;
    QTime end_time;
    QString classroom;

    QString subject_name;
    QString teacher_name;
    QString class_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}