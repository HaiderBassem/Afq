#pragma once

#include <QString>
#include <QStringList>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include "DataModel/BaseEntity.h"

namespace DataModel
{

enum class AttendanceStatus
{
    Present = 0,
    Absent = 1,
    Late = 2,
    Excused = 3,
    Vacation = 4,
    Unknown = -1
};


inline QString attendanceStatusToString(AttendanceStatus s)
{
    switch (s) {
    case AttendanceStatus::Present: return QObject::tr("Present");
    case AttendanceStatus::Absent: return QObject::tr("Absent");
    case AttendanceStatus::Late: return QObject::tr("Late");
    case AttendanceStatus::Excused: return QObject::tr("Excused");
    case AttendanceStatus::Vacation: return QObject::tr("Vacation");
    default: return QObject::tr("Unknown");
    }
}

inline AttendanceStatus intToAttendanceStatus(int i)
{
    if (i >= 0 && i <= 4) return static_cast<AttendanceStatus>(i);
    return AttendanceStatus::Unknown;
}

class Attendance : public BaseEntity
{
public:
    int attendance_id = 0;
    int student_enrollment_id = 0;
    QDate date;
    AttendanceStatus status = AttendanceStatus::Unknown;
    QStringList period_absent;
    QString reason;
    bool excuse_document = false;
    int created_by = 0;
    QDateTime created_at;

    // Additional fields for joins
    QString student_name;
    QString class_name;
    QString creator_name;

    bool isValid() const override;
    bool isExcused() const;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}