#include "DataModel/DisciplinaryAction.h"

namespace DataModel{
bool DisciplinaryAction::isValid() const
{
    return student_enrollment_id > 0 &&
           action_type != DisciplinaryActionType::Unknown &&
           !reason.isEmpty() &&
           action_date.isValid();
}

bool DisciplinaryAction::isActive() const
{
    if (duration_days <= 0) return false;
    return action_date.daysTo(QDate::currentDate()) < duration_days;
}

QJsonObject DisciplinaryAction::toJson() const
{
    QJsonObject obj;
    obj["action_id"] = action_id;
    obj["student_enrollment_id"] = student_enrollment_id;
    obj["action_type"] = static_cast<int>(action_type);
    obj["reason"] = reason;
    obj["action_date"] = action_date.toString(Qt::ISODate);
    obj["duration_days"] = duration_days;
    obj["issued_by"] = issued_by;
    obj["notes"] = notes;
    obj["parent_notified"] = parent_notified;
    obj["student_name"] = student_name;
    obj["issuer_name"] = issuer_name;
    obj["class_name"] = class_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    return obj;
}

void DisciplinaryAction::fromJson(const QJsonObject& json)
{
    action_id = json["action_id"].toInt();
    student_enrollment_id = json["student_enrollment_id"].toInt();
    action_type = intToDisciplinaryActionType(json["action_type"].toInt());
    reason = json["reason"].toString();
    action_date = QDate::fromString(json["action_date"].toString(), Qt::ISODate);
    duration_days = json["duration_days"].toInt();
    issued_by = json["issued_by"].toInt();
    notes = json["notes"].toString();
    parent_notified = json["parent_notified"].toBool();
    student_name = json["student_name"].toString();
    issuer_name = json["issuer_name"].toString();
    class_name = json["class_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
}

QString DisciplinaryAction::toString() const
{
    return QString("DisciplinaryAction[ID:%1, Student:%2, Type:%3, Date:%4]")
        .arg(action_id)
        .arg(student_enrollment_id)
        .arg(disciplinaryActionTypeToString(action_type))
        .arg(action_date.toString("yyyy-MM-dd"));
}
}