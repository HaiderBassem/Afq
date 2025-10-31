#include "DataModel/Schedule.h"

namespace DataModel
{
bool Schedule::isValid() const
{
    return offering_id > 0 &&
           period_number >= 1 && period_number <= 6 &&
           start_time.isValid() &&
           end_time.isValid() &&
           start_time < end_time &&
           !classroom.isEmpty();
}

QJsonObject Schedule::toJson() const
{
    QJsonObject obj;
    obj["schedule_id"] = schedule_id;
    obj["offering_id"] = offering_id;
    obj["day_of_week"] = static_cast<int>(day_of_week);
    obj["period_number"] = period_number;
    obj["start_time"] = start_time.toString(Qt::ISODate);
    obj["end_time"] = end_time.toString(Qt::ISODate);
    obj["classroom"] = classroom;
    obj["subject_name"] = subject_name;
    obj["teacher_name"] = teacher_name;
    obj["class_name"] = class_name;
    return obj;
}

void Schedule::fromJson(const QJsonObject& json)
{
    schedule_id = json["schedule_id"].toInt();
    offering_id = json["offering_id"].toInt();
    day_of_week = intToDayOfWeek(json["day_of_week"].toInt());
    period_number = json["period_number"].toInt();
    start_time = QTime::fromString(json["start_time"].toString(), Qt::ISODate);
    end_time = QTime::fromString(json["end_time"].toString(), Qt::ISODate);
    classroom = json["classroom"].toString();
    subject_name = json["subject_name"].toString();
    teacher_name = json["teacher_name"].toString();
    class_name = json["class_name"].toString();
}

QString Schedule::toString() const
{
    return QString("Schedule[ID:%1, Day:%2, Period:%3, Classroom:%4]")
        .arg(schedule_id)
        .arg(dayOfWeekToString(day_of_week))
        .arg(period_number)
        .arg(classroom);
}
}