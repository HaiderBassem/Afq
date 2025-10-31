#include "DataModel/Attendance.h"
namespace DataModel
{


bool Attendance::isValid() const
{
    return student_enrollment_id > 0 && date.isValid();
}

bool Attendance::isExcused() const
{
    return status == AttendanceStatus::Excused || excuse_document;
}

QJsonObject Attendance::toJson() const
{
    QJsonObject obj;
    obj["attendance_id"] = attendance_id;
    obj["student_enrollment_id"] = student_enrollment_id;
    obj["date"] = date.toString(Qt::ISODate);
    obj["status"] = static_cast<int>(status);

    QJsonArray periodsArray;
    for (const QString& period : period_absent)
        periodsArray.append(period);
    obj["period_absent"] = periodsArray;

    obj["reason"] = reason;
    obj["excuse_document"] = excuse_document;
    obj["created_by"] = created_by;
    obj["student_name"] = student_name;
    obj["class_name"] = class_name;
    obj["creator_name"] = creator_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    return obj;
}

void Attendance::fromJson(const QJsonObject& json)
{
    attendance_id = json["attendance_id"].toInt();
    student_enrollment_id = json["student_enrollment_id"].toInt();
    date = QDate::fromString(json["date"].toString(), Qt::ISODate);
    status = intToAttendanceStatus(json["status"].toInt());

    period_absent.clear();
    QJsonArray periodsArray = json["period_absent"].toArray();
    for (const QJsonValue& value : periodsArray)
        period_absent.append(value.toString());

    reason = json["reason"].toString();
    excuse_document = json["excuse_document"].toBool();
    created_by = json["created_by"].toInt();
    student_name = json["student_name"].toString();
    class_name = json["class_name"].toString();
    creator_name = json["creator_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
}

QString Attendance::toString() const
{
    return QString("Attendance[ID:%1, Student:%2, Date:%3, Status:%4]")
        .arg(attendance_id)
        .arg(student_enrollment_id)
        .arg(date.toString("yyyy-MM-dd"))
        .arg(attendanceStatusToString(status));
}
}