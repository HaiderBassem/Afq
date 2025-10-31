#ifndef STUDENT_GUARDIAN_H
#define STUDENT_GUARDIAN_H
#include <QString>
#include <QJsonObject>
#include "BaseEntity.h"
#include "DataModel/Guardian.h"
namespace DataModel
{
class StudentGuardian : public BaseEntity
{
public:
    int student_guardian_id = 0;
    int student_person_id = 0;
    int guardian_id = 0;
    QString relationship;
    bool is_primary = false;


    QString student_name;
    QString guardian_name;


    Guardian guardianInfo;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}
#endif