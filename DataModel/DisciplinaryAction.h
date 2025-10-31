#ifndef DISCIPLINARYACTION_H
#define DISCIPLINARYACTION_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel{

enum class DisciplinaryActionType
{
    Warning = 0,
    FirstWarning = 1,
    SecondWarning = 2,
    TemporaryDismissal = 3,
    Dismissal = 4,
    Deprivation = 5,
    Unknown = -1
};


inline QString disciplinaryActionTypeToString(DisciplinaryActionType t)
{
    switch (t) {
    case DisciplinaryActionType::Warning: return QObject::tr("Warning");
    case DisciplinaryActionType::FirstWarning: return QObject::tr("First Warning");
    case DisciplinaryActionType::SecondWarning: return QObject::tr("Second Warning");
    case DisciplinaryActionType::TemporaryDismissal: return QObject::tr("Temporary Dismissal");
    case DisciplinaryActionType::Dismissal: return QObject::tr("Dismissal");
    case DisciplinaryActionType::Deprivation: return QObject::tr("Deprivation");
    default: return QObject::tr("Unknown");
    }
}



inline DisciplinaryActionType intToDisciplinaryActionType(int i)
{
    if (i >= 0 && i <= 5) return static_cast<DisciplinaryActionType>(i);
    return DisciplinaryActionType::Unknown;
}


class DisciplinaryAction : public BaseEntity
{
public:
    int action_id = 0;
    int student_enrollment_id = 0;
    DisciplinaryActionType action_type = DisciplinaryActionType::Unknown;
    QString reason;
    QDate action_date;
    int duration_days = 0;
    int issued_by = 0;
    QString notes;
    bool parent_notified = false;
    QDateTime created_at;

    // Additional fields for joins
    QString student_name;
    QString issuer_name;
    QString class_name;

    bool isValid() const override;
    bool isActive() const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}
#endif // DISCIPLINARYACTION_H
