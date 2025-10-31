#include "DataModel/Term.h"

namespace DataModel
{
bool Term::isValid() const
{
    return !name.isEmpty() &&
           year_id > 0 &&
           start_date.isValid() &&
           end_date.isValid() &&
           start_date < end_date;
}

bool Term::isCurrentTerm() const
{
    QDate today = QDate::currentDate();
    return is_active && today >= start_date && today <= end_date;
}

int Term::getWeeksCount() const
{
    if (!start_date.isValid() || !end_date.isValid()) return 0;
    return start_date.daysTo(end_date) / 7;
}

QJsonObject Term::toJson() const
{
    QJsonObject obj;
    obj["term_id"] = term_id;
    obj["year_id"] = year_id;
    obj["name"] = name;
    obj["start_date"] = start_date.toString(Qt::ISODate);
    obj["end_date"] = end_date.toString(Qt::ISODate);
    obj["is_active"] = is_active;
    obj["exam_start_date"] = exam_start_date.toString(Qt::ISODate);
    obj["exam_end_date"] = exam_end_date.toString(Qt::ISODate);
    obj["year_name"] = year_name;
    return obj;
}

void Term::fromJson(const QJsonObject& json)
{
    term_id = json["term_id"].toInt();
    year_id = json["year_id"].toInt();
    name = json["name"].toString();
    start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
    end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
    is_active = json["is_active"].toBool();
    exam_start_date = QDate::fromString(json["exam_start_date"].toString(), Qt::ISODate);
    exam_end_date = QDate::fromString(json["exam_end_date"].toString(), Qt::ISODate);
    year_name = json["year_name"].toString();
}

QString Term::toString() const
{
    return QString("Term[ID:%1, Name:%2, Year:%3, Active:%4]")
        .arg(term_id)
        .arg(name)
        .arg(year_id)
        .arg(is_active ? "Yes" : "No");
}

}