#include"DataModel/Grade.h"

namespace DataModel
{
#include "Grade.h"

bool Grade::isValid() const
{
    return student_enrollment_id > 0 &&
           offering_id > 0 &&
           score >= 0 && score <= max_score &&
           exam_date.isValid();
}

bool Grade::isPassing() const
{
    return !is_absent && score >= 50.0; 
}

double Grade::getPercentage() const
{
    if (max_score == 0) return 0.0;
    return (score / max_score) * 100.0;
}

QJsonObject Grade::toJson() const
{
    QJsonObject obj;
    obj["grade_id"] = grade_id;
    obj["student_enrollment_id"] = student_enrollment_id;
    obj["offering_id"] = offering_id;
    obj["exam_type"] = static_cast<int>(exam_type);
    obj["score"] = score;
    obj["max_score"] = max_score;
    obj["weight"] = weight;
    obj["exam_date"] = exam_date.toString(Qt::ISODate);
    obj["is_absent"] = is_absent;
    obj["notes"] = notes;
    obj["graded_by"] = graded_by;
    obj["student_name"] = student_name;
    obj["subject_name"] = subject_name;
    obj["grader_name"] = grader_name;
    obj["class_name"] = class_name;
    obj["graded_at"] = graded_at.toString(Qt::ISODate);
    return obj;
}

void Grade::fromJson(const QJsonObject& json)
{
    grade_id = json["grade_id"].toInt();
    student_enrollment_id = json["student_enrollment_id"].toInt();
    offering_id = json["offering_id"].toInt();
    exam_type = intToExamType(json["exam_type"].toInt());
    score = json["score"].toDouble();
    max_score = json["max_score"].toDouble();
    weight = json["weight"].toDouble();
    exam_date = QDate::fromString(json["exam_date"].toString(), Qt::ISODate);
    is_absent = json["is_absent"].toBool();
    notes = json["notes"].toString();
    graded_by = json["graded_by"].toInt();
    student_name = json["student_name"].toString();
    subject_name = json["subject_name"].toString();
    grader_name = json["grader_name"].toString();
    class_name = json["class_name"].toString();
    graded_at = QDateTime::fromString(json["graded_at"].toString(), Qt::ISODate);
}

QString Grade::toString() const
{
    return QString("Grade[ID:%1, Student:%2, Subject:%3, Score:%4/%5]")
        .arg(grade_id)
        .arg(student_enrollment_id)
        .arg(offering_id)
        .arg(score)
        .arg(max_score);
}

}