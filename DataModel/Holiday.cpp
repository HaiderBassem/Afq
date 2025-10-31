#include "DataModel/Holiday.h"

namespace DataModel{
bool Holiday::isValid() const
{
    return !name.isEmpty() &&
           year_id > 0 &&
           start_date.isValid() &&
           end_date.isValid() &&
           start_date <= end_date;
}

bool Holiday::isCurrentHoliday() const
{
    QDate today = QDate::currentDate();
    return today >= start_date && today <= end_date;
}

int Holiday::getDurationInDays() const
{
    if (!start_date.isValid() || !end_date.isValid()) return 0;
    return start_date.daysTo(end_date) + 1;
}

QJsonObject Holiday::toJson() const
{
    QJsonObject obj;
    obj["holiday_id"] = holiday_id;
    obj["year_id"] = year_id;
    obj["name"] = name;
    obj["start_date"] = start_date.toString(Qt::ISODate);
    obj["end_date"] = end_date.toString(Qt::ISODate);
    obj["type"] = static_cast<int>(type);
    obj["is_planned"] = is_planned;
    obj["year_name"] = year_name;
    return obj;
}

void Holiday::fromJson(const QJsonObject& json)
{
    holiday_id = json["holiday_id"].toInt();
    year_id = json["year_id"].toInt();
    name = json["name"].toString();
    start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
    end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
    type = static_cast<HolidayType>(json["type"].toInt());
    is_planned = json["is_planned"].toBool();
    year_name = json["year_name"].toString();
}

QString Holiday::toString() const
{
    return QString("Holiday[ID:%1, Name:%2, Duration:%3 days]")
        .arg(holiday_id)
        .arg(name)
        .arg(getDurationInDays());
}
}