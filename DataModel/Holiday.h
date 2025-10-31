#ifndef HOLIDAY_H
#define HOLIDAY_H

#include <QString>
#include <QDate>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
    

enum class HolidayType
{
    Official = 0,
    Religious = 1,
    National = 2,
    Emergency = 3,
    Unknown = -1
};

inline QString holidayTypeToString(HolidayType t)
{
    switch (t) {
    case HolidayType::Official: return QObject::tr("Official");
    case HolidayType::Religious: return QObject::tr("Religious");
    case HolidayType::National: return QObject::tr("National");
    case HolidayType::Emergency: return QObject::tr("Emergency");
    default: return QObject::tr("Unknown");
    }
}

inline HolidayType intToHolidayType(int i)
{
    if (i >= 0 && i <= 3) return static_cast<HolidayType>(i);
    return HolidayType::Unknown;
}

class Holiday : public BaseEntity
{
public:
    int holiday_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    HolidayType type = HolidayType::Unknown;
    bool is_planned = true;

    // Additional fields for joins
    QString year_name;

    bool isValid() const override;
    bool isCurrentHoliday() const;
    int getDurationInDays() const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};

} // namespace DataModel



#endif // HOLIDAY_H
