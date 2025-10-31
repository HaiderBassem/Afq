#include "DataModel/Enrollment.h"


namespace DataModel
{


bool Enrollment::isValid() const
{
    return person_id > 0 &&
           role != EnrollmentRole::Unknown &&
           start_date.isValid();
}

bool Enrollment::isActive() const
{
    return status == EnrollmentStatus::Active &&
           (!end_date.isValid() || end_date >= QDate::currentDate());
}

int Enrollment::getDurationInDays() const
{
    if (!start_date.isValid()) return 0;
    QDate endDate = end_date.isValid() ? end_date : QDate::currentDate();
    return start_date.daysTo(endDate);
}

QJsonObject Enrollment::toJson() const
{
    QJsonObject obj;
    obj["enrollment_id"] = enrollment_id;
    obj["person_id"] = person_id;
    obj["enrollment_number"] = enrollment_number;
    obj["role"] = static_cast<int>(role);
    obj["start_date"] = start_date.toString(Qt::ISODate);
    obj["end_date"] = end_date.toString(Qt::ISODate);
    obj["status"] = static_cast<int>(status);
    obj["person_name"] = person_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    obj["updated_at"] = updated_at.toString(Qt::ISODate);
    return obj;
}

void Enrollment::fromJson(const QJsonObject& json)
{
    enrollment_id = json["enrollment_id"].toInt();
    person_id = json["person_id"].toInt();
    enrollment_number = json["enrollment_number"].toString();
    role = intToEnrollmentRole(json["role"].toInt());
    start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
    end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
    status = intToEnrollmentStatus(json["status"].toInt());
    person_name = json["person_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
}

QString Enrollment::toString() const
{
    return QString("Enrollment[ID:%1, Person:%2, Role:%3, Status:%4]")
        .arg(enrollment_id)
        .arg(person_id)
        .arg(enrollmentRoleToString(role))
        .arg(enrollmentStatusToString(status));
}
}