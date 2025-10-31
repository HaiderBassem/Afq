#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel
{
    


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
