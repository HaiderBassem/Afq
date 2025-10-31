#include "DataModel/AcademicYear.h"

namespace DataModel
{
bool AcademicYear::isValid() const
{
    return !name.isEmpty() &&
           start_date.isValid() &&
           end_date.isValid() &&
           start_date < end_date;
}

bool AcademicYear::isCurrentYear() const
{
    QDate today = QDate::currentDate();
    return is_active && today >= start_date && today <= end_date;
}

int AcademicYear::getDurationInDays() const
{
    if (!start_date.isValid() || !end_date.isValid()) return 0;
    return start_date.daysTo(end_date);
}

QJsonObject AcademicYear::toJson() const
{
    QJsonObject obj;
    obj["year_id"] = year_id;
    obj["name"] = name;
    obj["start_date"] = start_date.toString(Qt::ISODate);
    obj["end_date"] = end_date.toString(Qt::ISODate);
    obj["is_active"] = is_active;
    obj["holidays_count"] = holidays_count;
    return obj;
}

void AcademicYear::fromJson(const QJsonObject& json)
{
    year_id = json["year_id"].toInt();
    name = json["name"].toString();
    start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
    end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
    is_active = json["is_active"].toBool();
    holidays_count = json["holidays_count"].toInt();
}

QString AcademicYear::toString() const
{
    return QString("AcademicYear[ID:%1, Name:%2, Active:%3]")
        .arg(year_id)
        .arg(name)
        .arg(is_active ? "Yes" : "No");
}
}