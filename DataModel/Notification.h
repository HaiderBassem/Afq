#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"


namespace DataModel

{
    
enum class NotificationType
{
    Grades = 0,
    Attendance = 1,
    Conduct = 2,
    Unknown = -1
};

enum class NotificationPriority
{
    Normal = 0,
    Important = 1,
    VeryImportant = 2,
    Unknown = -1
};




inline QString notificationTypeToString(NotificationType t)
{
    switch (t)
    {
    case NotificationType::Grades: return QObject::tr("Grades");
    case NotificationType::Attendance: return QObject::tr("Attendance");
    case NotificationType::Conduct: return QObject::tr("Conduct");
    default: return QObject::tr("Unknown");
    }
}

inline NotificationType intToNotificationType(int i)
{
    if (i >= 0 && i <= 2) return static_cast<NotificationType>(i);
    return NotificationType::Unknown;
}



// NotificationPriority conversions
inline QString notificationPriorityToString(NotificationPriority p)
{
    switch (p)
    {
    case NotificationPriority::Normal: return QObject::tr("Normal");
    case NotificationPriority::Important: return QObject::tr("Important");
    case NotificationPriority::VeryImportant: return QObject::tr("Very Important");
    default: return QObject::tr("Unknown");
    }
}

inline NotificationPriority intToNotificationPriority(int i)
{
    if (i >= 0 && i <= 2) return static_cast<NotificationPriority>(i);
    return NotificationPriority::Unknown;
}



class Notification : public BaseEntity
{
public:
    int notification_id = 0;
    QString title;
    QString content;
    NotificationType type = NotificationType::Unknown;
    NotificationPriority priority = NotificationPriority::Normal;
    int sender_id = 0;
    QDateTime created_at;

    // Additional fields for joins
    QString sender_name;

    bool isValid() const override;
    bool isImportant() const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};

} // namespace DataModel

#endif // NOTIFICATION_H
