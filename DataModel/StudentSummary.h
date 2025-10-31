#ifndef STUDENTSUMMARY_H
#define STUDENTSUMMARY_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "DataModel/BaseEntity.h"

namespace DataModel {

class StudentSummary : public QObject, public BaseEntity
{
    Q_OBJECT
public:
    explicit StudentSummary(QObject *parent = nullptr);
    bool isValid() const override;
    double getCompletionRate() const;
    QString getFullName() const noexcept;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

private:
    int student_id = 0;
    QString student_number;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    QString class_name;
    QString section;
    QString status;
    double average = 0.0;
    int rank = 0;
    int attendance_rate = 0;
    int completed_subjects = 0;
    int total_subjects = 0;
    int total_absences = 0;
};

}

#endif // STUDENTSUMMARY_H
