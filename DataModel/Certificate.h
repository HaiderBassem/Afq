#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
    


enum class CertificateType {
    Completion,
    Excellence,
    Attendance,
    Merit,
    Thanks,
    Unknown
};

inline QString certificateTypeToString(CertificateType type) {
    switch(type) {
        case CertificateType::Completion: return "Completion";
        case CertificateType::Excellence: return "Excellence";
        case CertificateType::Attendance: return "Attendance";
        case CertificateType::Merit: return "Merit";
        case CertificateType::Thanks: return "Thanks";
        default: return "Unknown";
    }
}

inline CertificateType intToCertificateType(int value) {
    switch(value) {
        case 0: return CertificateType::Completion;
        case 1: return CertificateType::Excellence;
        case 2: return CertificateType::Attendance;
        case 3: return CertificateType::Merit;
        case 4: return CertificateType::Thanks;
        default: return CertificateType::Unknown;
    }
}


class Certificate : public BaseEntity
{
public:
    int certificate_id = 0;
    int student_enrollment_id = 0;
    CertificateType certificate_type = CertificateType::Unknown;
    QDate issue_date;
    QString certificate_number;
    int issued_by = 0;
    QString notes;
    QDateTime created_at;

    // Additional fields for joins
    QString student_name;
    QString issuer_name;
    QString class_name;

    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
} // namespace DataModel


#endif // CERTIFICATE_H
