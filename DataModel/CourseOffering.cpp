#include "DataModel/CourseOffering.h"
namespace DataModel
{
bool CourseOffering::isValid() const
{
    return curriculum_id > 0 &&
           teacher_enrollment_id > 0 &&
           term_id > 0;
}

QJsonObject CourseOffering::toJson() const
{
    QJsonObject obj;
    obj["offering_id"] = offering_id;
    obj["curriculum_id"] = curriculum_id;
    obj["teacher_enrollment_id"] = teacher_enrollment_id;
    obj["term_id"] = term_id;
    obj["current_students"] = current_students;
    obj["subject_name"] = subject_name;
    obj["teacher_name"] = teacher_name;
    obj["class_name"] = class_name;
    obj["term_name"] = term_name;
    return obj;
}

void CourseOffering::fromJson(const QJsonObject& json)
{
    offering_id = json["offering_id"].toInt();
    curriculum_id = json["curriculum_id"].toInt();
    teacher_enrollment_id = json["teacher_enrollment_id"].toInt();
    term_id = json["term_id"].toInt();
    current_students = json["current_students"].toInt();
    subject_name = json["subject_name"].toString();
    teacher_name = json["teacher_name"].toString();
    class_name = json["class_name"].toString();
    term_name = json["term_name"].toString();
}

QString CourseOffering::toString() const
{
    return QString("CourseOffering[ID:%1, Curriculum:%2, Teacher:%3, Students:%4]")
        .arg(offering_id)
        .arg(curriculum_id)
        .arg(teacher_enrollment_id)
        .arg(current_students);
}
}