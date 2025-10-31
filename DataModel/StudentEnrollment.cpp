
#include"DataModel/Enrollment.h"
#include"DataModel/StudentEnrollment.h"
namespace DataModel
{

inline EnrollmentRole intToEnrollmentRole(int i)
{
    if (i >= 0 && i <= 3) return static_cast<EnrollmentRole>(i);
    return EnrollmentRole::Unknown;
}
inline EnrollmentStatus intToEnrollmentStatus(int i)
{
    if (i >= 0 && i <= 7) return static_cast<EnrollmentStatus>(i);
    return EnrollmentStatus::Unknown;
}

inline QString enrollmentRoleToString(EnrollmentRole r)
{
    switch (r) {
    case EnrollmentRole::Student: return QObject::tr("Student");
    case EnrollmentRole::Teacher: return QObject::tr("Teacher");
    case EnrollmentRole::Staff: return QObject::tr("Staff");
    case EnrollmentRole::Administrator: return QObject::tr("Administrator");
    default: return QObject::tr("Unknown");
    }
}

inline QString enrollmentStatusToString(EnrollmentStatus s)
{
    switch (s) {
    case EnrollmentStatus::Active: return QObject::tr("Active");
    case EnrollmentStatus::Inactive: return QObject::tr("Inactive");
    case EnrollmentStatus::Graduate: return QObject::tr("Graduate");
    case EnrollmentStatus::Transferred: return QObject::tr("Transferred");
    case EnrollmentStatus::Dismissed: return QObject::tr("Dismissed");
    case EnrollmentStatus::Withdrawn: return QObject::tr("Withdrawn");
    case EnrollmentStatus::Failed: return QObject::tr("Failed");
    case EnrollmentStatus::Postponed: return QObject::tr("Postponed");
    default: return QObject::tr("Unknown");
    }
}


bool StudentEnrollment::isValid() const
{
    return enrollment_id > 0 &&
           class_id > 0 &&
           year_id > 0 &&
           start_date.isValid();
}


bool StudentEnrollment::shouldPromote() const
    {
        return status == StudentStatus::Active && !is_repeated && final_average >= 50.0;
    }



QJsonObject StudentEnrollment::toJson() const
{
    QJsonObject obj;
    obj["student_enrollment_id"] = student_enrollment_id;
    obj["enrollment_id"] = enrollment_id;
    obj["class_id"] = class_id;
    obj["year_id"] = year_id;
    obj["start_date"] = start_date.toString(Qt::ISODate);
    obj["end_date"] = end_date.toString(Qt::ISODate);
    obj["status"] = static_cast<int>(status);
    obj["repeat_count"] = repeat_count;
    obj["is_graduated"] = is_graduated;
    obj["is_repeated"] = is_repeated;
    obj["final_average"] = final_average;
    obj["rank_in_class"] = rank_in_class;
    obj["is_ministerial_exam"] = is_ministerial_exam;
    obj["is_eligible_for_exam"] = is_eligible_for_exam;
    obj["student_name"] = student_name;
    obj["class_name"] = class_name;
    obj["section"] = section;
    obj["year_name"] = year_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    obj["updated_at"] = updated_at.toString(Qt::ISODate);
    return obj;
}



void StudentEnrollment::fromJson(const QJsonObject& json)
{
    student_enrollment_id = json["student_enrollment_id"].toInt();
    enrollment_id = json["enrollment_id"].toInt();
    class_id = json["class_id"].toInt();
    year_id = json["year_id"].toInt();
    start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
    end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
    status = static_cast<StudentStatus>(json["status"].toInt());
    repeat_count = json["repeat_count"].toInt();
    is_graduated = json["is_graduated"].toBool();
    is_repeated = json["is_repeated"].toBool();
    final_average = json["final_average"].toDouble();
    rank_in_class = json["rank_in_class"].toInt();
    is_ministerial_exam = json["is_ministerial_exam"].toBool();
    is_eligible_for_exam = json["is_eligible_for_exam"].toBool();
    student_name = json["student_name"].toString();
    class_name = json["class_name"].toString();
    section = json["section"].toString();
    year_name = json["year_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
}


QString StudentEnrollment::toString() const
{
    return QString("StudentEnrollment[ID:%1, Student:%2, Class:%3, Status:%4]")
        .arg(student_enrollment_id)
        .arg(enrollment_id)
        .arg(class_id)
        .arg(static_cast<int>(status));
}
}