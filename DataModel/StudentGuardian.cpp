#include "DataModel/StudentGuardian.h"

namespace DataModel
{
bool StudentGuardian::isValid() const
{
    return student_guardian_id >= 0 &&
           student_person_id > 0 &&
           guardian_id > 0 &&
           !relationship.isEmpty();
}

QJsonObject StudentGuardian::toJson() const
{
    QJsonObject obj;
    obj["student_guardian_id"] = student_guardian_id;
    obj["student_person_id"] = student_person_id;
    obj["guardian_id"] = guardian_id;
    obj["relationship"] = relationship;
    obj["is_primary"] = is_primary;
    obj["student_name"] = student_name;
    obj["guardian_name"] = guardian_name;

    // Include guardian info (optional)
    obj["guardian_info"] = guardianInfo.toJson();

    return obj;
}

void StudentGuardian::fromJson(const QJsonObject& json)
{
    student_guardian_id = json["student_guardian_id"].toInt();
    student_person_id = json["student_person_id"].toInt();
    guardian_id = json["guardian_id"].toInt();
    relationship = json["relationship"].toString();
    is_primary = json["is_primary"].toBool();
    student_name = json["student_name"].toString();
    guardian_name = json["guardian_name"].toString();

    if (json.contains("guardian_info") && json["guardian_info"].isObject()) {
        guardianInfo.fromJson(json["guardian_info"].toObject());
    }
}

QString StudentGuardian::toString() const
{
    return QString("StudentGuardian[ID:%1, Student:%2, Guardian:%3, Relation:%4]")
        .arg(student_guardian_id)
        .arg(student_person_id)
        .arg(guardian_id)
        .arg(relationship);
}
}