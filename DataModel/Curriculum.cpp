#include "DataModel/Curriculum.h"

namespace DataModel
{
bool Curriculum::isValid() const
{
    return class_id > 0 &&
           subject_id > 0 &&
           year_id > 0;
}

QJsonObject Curriculum::toJson() const
{
    QJsonObject obj;
    obj["curriculum_id"] = curriculum_id;
    obj["class_id"] = class_id;
    obj["subject_id"] = subject_id;
    obj["year_id"] = year_id;
    obj["textbook_name"] = textbook_name;
    obj["class_name"] = class_name;
    obj["subject_name"] = subject_name;
    obj["year_name"] = year_name;
    return obj;
}

void Curriculum::fromJson(const QJsonObject& json)
{
    curriculum_id = json["curriculum_id"].toInt();
    class_id = json["class_id"].toInt();
    subject_id = json["subject_id"].toInt();
    year_id = json["year_id"].toInt();
    textbook_name = json["textbook_name"].toString();
    class_name = json["class_name"].toString();
    subject_name = json["subject_name"].toString();
    year_name = json["year_name"].toString();
}

QString Curriculum::toString() const
{
    return QString("Curriculum[ID:%1, Class:%2, Subject:%3, Year:%4]")
        .arg(curriculum_id)
        .arg(class_id)
        .arg(subject_id)
        .arg(year_id);
}
}