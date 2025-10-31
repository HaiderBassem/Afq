#include "DataModel/Notification.h"
namespace DataModel
{
    

bool Notification::isValid() const
{
    return !title.isEmpty() &&
           !content.isEmpty();
}

bool Notification::isImportant() const
{
    return priority == NotificationPriority::Important ||
           priority == NotificationPriority::VeryImportant;
}

QJsonObject Notification::toJson() const
{
    QJsonObject obj;
    obj["notification_id"] = notification_id;
    obj["title"] = title;
    obj["content"] = content;
    obj["type"] = static_cast<int>(type);
    obj["priority"] = static_cast<int>(priority);
    obj["sender_id"] = sender_id;
    obj["sender_name"] = sender_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    return obj;
}

void Notification::fromJson(const QJsonObject& json)
{
    notification_id = json["notification_id"].toInt();
    title = json["title"].toString();
    content = json["content"].toString();
    type = static_cast<NotificationType>(json["type"].toInt());
    priority = static_cast<NotificationPriority>(json["priority"].toInt());
    sender_id = json["sender_id"].toInt();
    sender_name = json["sender_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
}

QString Notification::toString() const
{
    return QString("Notification[ID:%1, Title:%2, Priority:%3]")
        .arg(notification_id)
        .arg(title)
        .arg(static_cast<int>(priority));
}
} // namespace DataModel
