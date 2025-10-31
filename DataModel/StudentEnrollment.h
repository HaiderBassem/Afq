#ifndef STUDENT_ENROLLMENT_H
#define STUDENT_ENROLLMENT_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>


#include"DataModel/BaseEntity.h"
#include "DataModel/Student.h"

namespace DataModel
{

class StudentEnrollment : public BaseEntity
{
public:
    int student_enrollment_id = 0;
    int enrollment_id = 0;
    int class_id = 0;
    int year_id = 0;
    QDate start_date;
    QDate end_date;
    StudentStatus status = StudentStatus::Unknown;
    int repeat_count = 0;
    bool is_graduated = false;
    bool is_repeated = false;
    double final_average = 0.0;
    int rank_in_class = 0;
    bool is_ministerial_exam = false;
    bool is_eligible_for_exam = false;
    QDateTime created_at;
    QDateTime updated_at;

    QString student_name;
    QString class_name;
    QString section;
    QString year_name;


    bool isValid() const override;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;

    bool shouldPromote() const;
};
}
#endif