#include "DataModel/Certificate.h"

namespace DataModel
{
    

    

bool Certificate::isValid() const
{
    return student_enrollment_id > 0 &&
           certificate_type != CertificateType::Unknown &&
           issue_date.isValid() &&
           !certificate_number.isEmpty();
}

QJsonObject Certificate::toJson() const
{
    QJsonObject obj;
    obj["certificate_id"] = certificate_id;
    obj["student_enrollment_id"] = student_enrollment_id;
    obj["certificate_type"] = static_cast<int>(certificate_type);
    obj["issue_date"] = issue_date.toString(Qt::ISODate);
    obj["certificate_number"] = certificate_number;
    obj["issued_by"] = issued_by;
    obj["notes"] = notes;
    obj["student_name"] = student_name;
    obj["issuer_name"] = issuer_name;
    obj["class_name"] = class_name;
    obj["created_at"] = created_at.toString(Qt::ISODate);
    return obj;
}

void Certificate::fromJson(const QJsonObject& json)
{
    certificate_id = json["certificate_id"].toInt();
    student_enrollment_id = json["student_enrollment_id"].toInt();
    certificate_type = static_cast<CertificateType>(json["certificate_type"].toInt());
    issue_date = QDate::fromString(json["issue_date"].toString(), Qt::ISODate);
    certificate_number = json["certificate_number"].toString();
    issued_by = json["issued_by"].toInt();
    notes = json["notes"].toString();
    student_name = json["student_name"].toString();
    issuer_name = json["issuer_name"].toString();
    class_name = json["class_name"].toString();
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
}

QString Certificate::toString() const
{
    return QString("Certificate[ID:%1, Student:%2, Type:%3, Number:%4]")
        .arg(certificate_id)
        .arg(student_enrollment_id)
        .arg(static_cast<int>(certificate_type))
        .arg(certificate_number);
}
} // namespace DataModel
