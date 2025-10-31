#pragma once

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"


namespace DataModel
{

enum class EnrollmentRole
{
    Student = 0,
    Teacher = 1,
    Staff = 2,
    Administrator = 3,
    Unknown = -1
};

// Enum: Enrollment Status
enum class EnrollmentStatus
{
    Inactive = 0,
    Active = 1,
    Graduate = 2,
    Transferred = 3,
    Dismissed = 4,
    Withdrawn = 5,
    Failed = 6,
    Postponed = 7,
    Unknown = -1
};

// ---- Utility conversion functions ----
inline QString enrollmentRoleToString(EnrollmentRole r)
{
    switch (r) 
    {
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



class Enrollment : public BaseEntity
{
public:
    int enrollment_id = 0;
    int person_id = 0;
    QString enrollment_number;
    EnrollmentRole role = EnrollmentRole::Unknown;
    QDate start_date;
    QDate end_date;
    EnrollmentStatus status = EnrollmentStatus::Unknown;
    QDateTime created_at;
    QDateTime updated_at;


    QString person_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;

    // ---- Custom logic ----
    bool isActive() const;
    int getDurationInDays() const;
};
}