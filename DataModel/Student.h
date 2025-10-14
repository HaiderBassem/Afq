#ifndef STUDENT_H
#define STUDENT_H

#include"DataModel.h"

#include <QObject>
namespace DataModel {


enum class StudentStatus
{
    Inactive = 0,
    Active = 1,
    Graduate = 2,
    Transferred = 3,
    Dismissed = 4,
    Withdrawn = 5,
    Failed = 6,
    E_Learning = 7,

    Unknown = -1
};

enum class StudentType
{
    Regular = 0,
    Honors = 1,
    SpecialNeeds = 2,
    Week = 3,
    Unknown = -1
};



inline QString studentToString(StudentStatus status)
{
    switch (status)
    {
    case StudentStatus::Active: return QObject::tr("نشط");
    case StudentStatus::Inactive: return QObject::tr("غير نشط");
    case StudentStatus::Graduate: return QObject::tr("خريج");
    case StudentStatus::Active: return QObject::tr("نشط");
    case StudentStatus::Transferred: return QObject::tr("منقول");
    case StudentStatus::Dismissed: return QObject::tr("مفصول");
    case StudentStatus::Withdrawn: return QObject::tr("منسحب");
    case StudentStatus::Failed: return QObject::tr("راسب");
    case StudentStatus::E_Learning: return QObject::tr("انتساب");
    case StudentStatus::Unknown: return QObject::tr("غير معروف");

    default:
        case StudentStatus::Unknown: return QObject::tr("غير معروف");
    }
}

inline StudentStatus intToStudentStatus(int value)
{
    if(value >= 0 && value <= 7)
        return static_cast<StudentStatus>(value);
    return StudentStatus::Unknown;
}

inline QString studentTypeToString(StudentType type)
{
    switch (type)
    {
    case StudentType::Regular: return QObject::tr("عادي");
    case StudentType::Honors: return QObject::tr("متفوق");
    case StudentType::Week: return QObject::tr("مو اكاديمي");
    case StudentType::SpecialNeeds: return QObject::tr("احتياجات خاصة");
    case StudentType::Unknown: return QObject::tr("غير معروف");
    default:
        case StudentStatus::Unknown: return QObject::tr("غير معروف");
    }
}


class Student : public QObject, public BaseEntity
{

public:
    explicit Student(QObject *parent = nullptr);


    QString getFullName() const;

    bool isValid()() const;
    bool isActive() const;
    bool isHonorsStudent() const;
    bool isRegularStudent() const;
    bool isWeekStudent() const;
    bool isSpecialNeedsStudent() const;

    // josn serialization

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;





private:

    int student_id = 0;
    int person_id =0;

    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    QString full_name; // I will complete it tmrw
    DataModel::Gender gender = DataModel::Gender::Male;
    QDate date_of_birth;
    int age = calculateAge();

    QString student_number;
    StudentStatus status = StudentStatus::Active;
    StudentType type = StudentType::Regular;
    QDate enrollment_date;
    QDate graduation_date;
    QString phone;






    int current_class_id =0;
    QString current_class_name ;
    int current_year_id =0;
    QString current_year_name;
    double current_average= 0.0;
    int current_rank = 0;

    int calculateAge() const;




};
}



class StudentSummary : public QObject, public BaseEntity
{
public:
    explicit StudentSummary(QObject *parent = nullptr);
    bool isValid() const override ;
    double getCompletionRate() const;
    QJsonObject toJson() const override;
    void fromJson() override;
    QString toString() const override;



private:
    int student_id;
    QString student_number;
    QString full_name;
    QString class_name;
    QString status;
    double avarage= 0.0;
    int rank = 0;
    int attendance_rate =0;
    int completed_subjecs =0;
    int total_subjects = 0;

};

#endif // STUDENT_H



































