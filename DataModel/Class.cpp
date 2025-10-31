#include "DataModel/Class.h"
namespace DataModel
{

bool Class::isValid() const
{
    return !name.isEmpty() &&
           stage_id > 0 &&
           year > 2000 &&
           grade_level >= 1 && grade_level <= 12;
}

QString Class::getFullClassName() const
{
    if (section.isEmpty()) return name;
    return QString("%1 - %2").arg(name).arg(section);
}

QJsonObject Class::toJson() const
{
    QJsonObject obj;
    obj["class_id"] = class_id;
    obj["stage_id"] = stage_id;
    obj["grade_level"] = grade_level;
    obj["name"] = name;
    obj["section"] = section;
    obj["year"] = year;
    obj["has_ministerial_exam"] = has_ministerial_exam;
    obj["stage_name"] = stage_name;
    return obj;
}

void Class::fromJson(const QJsonObject& json)
{
    class_id = json["class_id"].toInt();
    stage_id = json["stage_id"].toInt();
    grade_level = json["grade_level"].toInt();
    name = json["name"].toString();
    section = json["section"].toString();
    year = json["year"].toInt();
    has_ministerial_exam = json["has_ministerial_exam"].toBool();
    stage_name = json["stage_name"].toString();
}

QString Class::toString() const
{
    return QString("Class[ID:%1, Name:%2, Section:%3, Grade:%4]")
        .arg(class_id)
        .arg(name)
        .arg(section)
        .arg(grade_level);
}


}