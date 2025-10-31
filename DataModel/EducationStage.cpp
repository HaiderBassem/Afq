#include "DataModel/EducationStage.h"

namespace DataModel
{
bool EducationStage::isValid() const
{
    return !name.isEmpty() &&
           min_grade > 0 &&
           max_grade >= min_grade &&
           max_grade <= 12;
}

bool EducationStage::containsGrade(int grade) const
{
    return grade >= min_grade && grade <= max_grade;
}

QJsonObject EducationStage::toJson() const
{
    QJsonObject obj;
    obj["stage_id"] = stage_id;
    obj["name"] = name;
    obj["min_grade"] = min_grade;
    obj["max_grade"] = max_grade;
    obj["has_ministerial_exam"] = has_ministerial_exam;
    return obj;
}

void EducationStage::fromJson(const QJsonObject& json)
{
    stage_id = json["stage_id"].toInt();
    name = json["name"].toString();
    min_grade = json["min_grade"].toInt();
    max_grade = json["max_grade"].toInt();
    has_ministerial_exam = json["has_ministerial_exam"].toBool();
}

QString EducationStage::toString() const
{
    return QString("EducationStage[ID:%1, Name:%2, Grades:%3-%4]")
        .arg(stage_id)
        .arg(name)
        .arg(min_grade)
        .arg(max_grade);
}
}