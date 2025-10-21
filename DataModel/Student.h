#ifndef STUDENT_H
#define STUDENT_H

#include"DataModel.h"

#include <QObject>
#include<QString>
#include<QChar>
#include<optional>

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
    Weak = 3,
    Unknown = -1
};



inline QString studentStatusToString(StudentStatus status)
{
    switch (status)
    {
    case StudentStatus::Active: return QObject::tr("نشط");
    case StudentStatus::Inactive: return QObject::tr("غير نشط");
    case StudentStatus::Graduate: return QObject::tr("خريج");
    case StudentStatus::Transferred: return QObject::tr("منقول");
    case StudentStatus::Dismissed: return QObject::tr("مفصول");
    case StudentStatus::Withdrawn: return QObject::tr("منسحب");
    case StudentStatus::Failed: return QObject::tr("راسب");
    case StudentStatus::E_Learning: return QObject::tr("انتساب");
    case StudentStatus::Unknown: return QObject::tr("غير معروف");
    default:
        return QObject::tr("غير معروف");
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
    case StudentType::Weak: return QObject::tr("مو اكاديمي");
    case StudentType::SpecialNeeds: return QObject::tr("احتياجات خاصة");
    case StudentType::Unknown: return QObject::tr("غير معروف");
    default:
        return QObject::tr("غير معروف");
    }
}


inline QChar genderToQChar(Gender gender) 
{
    return static_cast<char>(gender);
}

inline Gender qCharToGender(QChar ch) 
{
    switch (ch.toLatin1()) {
        case 'M': return Gender::Male;
        case 'F': return Gender::Female;
        default: return Gender::Male; // default case
    }
}


class Student : public QObject, public BaseEntity
{

public:
    explicit Student(QObject *parent = nullptr);
    Student(const Student& other);
    Student& operator=(const Student& other);
    Student(Student&& other) noexcept;
    Student& operator=(Student&& other) noexcept;
    ~Student() override;



    QString getFullName() const noexcept;

    bool isValid() const override;

    bool isActive() const;
    bool isHonorsStudent() const;
    bool isRegularStudent() const;
    bool isWeekStudent() const;
    bool isSpecialNeedsStudent() const;
    bool set_is_graduated(bool graduated);
    bool set_is_repeated(bool repeated);
    bool set_is_ministerial_exam(bool ministerial);
    bool set_is_eligible_for_exam(bool eligible);
    QString toString() const override;

    // josn serialization

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;


    // get proparity
    int get_student_id() const;
    int get_person_id() const;
    QString get_student_number() const;
    QString get_first_name() const;
    QString get_second_name() const;
    QString get_third_name() const;
    QString get_fourth_name() const;
    DataModel::Gender get_gender();
    QDate get_date_of_birth() const;
    int get_age() const;
    StudentStatus get_status() const;
    StudentType get_type() const;
    QDate get_enrollment_date() const;
    QDate get_graduation_date() const;
    QString get_phone() const;
    int get_current_class_id() const;
    QString get_current_class_name() const;
    QString get_section() const;
    int get_current_year_id() const;
    QString get_current_year_name() const;
    double get_current_average() const;
    int get_current_rank() const;
    int get_total_absences() const;



    bool set_student_id(int student_id);
    bool set_person_id(int person_id);
    bool set_first_name(const QString& fname);
    bool set_second_name(const QString& sname);
    bool set_third_name(const QString& tname) ;
    bool set_fourth_name(const QString& ftname);
    bool set_gender(QChar gender) ;
    bool set_date_of_birth(const QDate& dob) ;
    bool set_status(const StudentStatus& status);
    bool set_enrollment_date(const QDate& ed);
    bool set_graduation_date(const QDate& gd);
    bool set_phone(const QString& phone);
    bool set_age(int age);
    bool set_current_class_id(int class_id);
    bool set_current_class_name(const QString& class_name);
    bool set_section(const QString& section);
    bool set_current_year_id(int year_id);
    bool set_current_year_name(const QString& year_name);
    bool set_current_average(double average);
    bool set_current_rank(int rank);
    bool set_repeat_count(int);
    bool set_student_number(const QString& student_number);
    bool set_grade_level(int level);

private:


    int student_id = 0;
    int person_id =0;
    QString student_number;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    DataModel::Gender gender;
    QDate date_of_birth;
    int age = calculateAge();

    QString student_number;
    StudentStatus status = StudentStatus::Active;
    StudentType type = StudentType::Regular;
    QDate enrollment_date;
    QDate graduation_date;
    QString phone;

    bool is_graduated = false;
    bool is_repeated = false;
    bool is_ministerial_exam = false;
    bool is_eligible_for_exam = false;
 





    int current_class_id =0;
    QString current_class_name ;
    QString section;
    int current_year_id =0;
    QString current_year_name;
    double current_average= 0.0;
    int current_rank = 0;
    int grade_level;
    int repeat_count =0;
    int calculateAge() const;
    int total_absences = 0;




};






class StudentSummary : public QObject, public BaseEntity
{

public:
    explicit StudentSummary(QObject *parent = nullptr);
    bool isValid() const override ;
    double getCompletionRate() const;
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString getFullName() const noexcept;



private:
    int student_id;
    QString student_number;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    QString class_name;
    QString section;
    QString status;
    double average= 0.0;
    int rank = 0;
    int attendance_rate =0;
    int completed_subjects =0;
    int total_subjects = 0;

    int total_absences = 0;


};
}
#endif // STUDENT_H



































