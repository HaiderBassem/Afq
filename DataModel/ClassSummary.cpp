#include "DataModel/ClassSummary.h"

namespace DataModel
{
    


bool ClassSummary::isValid() const
{
    return class_id > 0 && !class_name.isEmpty();
}

QJsonObject ClassSummary::toJson() const
{
    QJsonObject obj;
    obj["class_id"] = class_id;
    obj["class_name"] = class_name;
    obj["section"] = section;
    obj["total_students"] = total_students;
    obj["active_students"] = active_students;
    obj["class_average"] = class_average;
    obj["subjects_count"] = subjects_count;
    obj["teacher_name"] = teacher_name;
    return obj;
}

void ClassSummary::fromJson(const QJsonObject& json)
{
    class_id = json["class_id"].toInt();
    class_name = json["class_name"].toString();
    section = json["section"].toString();
    total_students = json["total_students"].toInt();
    active_students = json["active_students"].toInt();
    class_average = json["class_average"].toDouble();
    subjects_count = json["subjects_count"].toInt();
    teacher_name = json["teacher_name"].toString();
}

QString ClassSummary::toString() const
{
    return QString("ClassSummary[ID:%1, Name:%2, Students:%3, Average:%4]")
        .arg(class_id)
        .arg(class_name)
        .arg(active_students)
        .arg(class_average);
}
} // namespace DataModel
