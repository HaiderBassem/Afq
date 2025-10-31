#include "DataModel/Subject.h"
namespace DataModel
{
bool Subject::isValid() const
{
    return !name.isEmpty() &&
           passing_grade >= 0 && passing_grade <= 100;
}

QJsonObject Subject::toJson() const
{
    QJsonObject obj;
    obj["subject_id"] = subject_id;
    obj["name"] = name;
    obj["code"] = code;
    obj["passing_grade"] = passing_grade;
    obj["stage_id"] = stage_id;
    obj["stage_name"] = stage_name;
    return obj;
}

void Subject::fromJson(const QJsonObject& json)
{
    subject_id = json["subject_id"].toInt();
    name = json["name"].toString();
    code = json["code"].toString();
    passing_grade = json["passing_grade"].toDouble();
    stage_id = json["stage_id"].toInt();
    stage_name = json["stage_name"].toString();
}

QString Subject::toString() const
{
    return QString("Subject[ID:%1, Name:%2, Code:%3, PassingGrade:%4]")
        .arg(subject_id)
        .arg(name)
        .arg(code)
        .arg(passing_grade);
}
}