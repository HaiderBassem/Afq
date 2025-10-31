#include "DataModel/Guardian.h"

namespace DataModel
{
bool Guardian::isValid() const
{
    return guardian_id >= 0 && person_id > 0;
}

QString Guardian::getFullName() const
{
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}


QJsonObject Guardian::toJson() const
{
    QJsonObject obj;
    obj["guardian_id"] = guardian_id;
    obj["person_id"] = person_id;
    obj["occupation"] = occupation;
    obj["work_phone"] = work_phone;
    obj["education_level"] = education_level;
    obj["guardian_name"] = guardian_name;
    obj["first_name"] = first_name;
    obj["second_name"] = second_name;
    obj["third_name"] = third_name;
    obj["fourth_name"] = fourth_name;
    obj["full_name"] = getFullName();
    obj["gender"] = QString(gender);
    obj["relationship"] = relationship;
    obj["isPrimary"] = isPrimary;
    obj["className"] = className;
    return obj;
}

void Guardian::fromJson(const QJsonObject& json)
{
    guardian_id = json["guardian_id"].toInt();
    person_id = json["person_id"].toInt();
    occupation = json["occupation"].toString();
    work_phone = json["work_phone"].toString();
    education_level = json["education_level"].toString();
    guardian_name = json["guardian_name"].toString();
    first_name = json["first_name"].toString();
    second_name = json["second_name"].toString();
    third_name = json["third_name"].toString();
    fourth_name = json["fourth_name"].toString();
    full_name = json["full_name"].toString();
    gender = json["gender"].toString().isEmpty() ? QChar() : json["gender"].toString()[0];
    relationship = json["relationship"].toString();
    isPrimary = json["isPrimary"].toBool();
    className = json["className"].toString();
}

QString Guardian::toString() const
{
    return QString("Guardian[ID:%1, PersonID:%2, Occupation:%3]")
        .arg(guardian_id)
        .arg(person_id)
        .arg(occupation);
}
}