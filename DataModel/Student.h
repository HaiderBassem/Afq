#ifndef DATAMODEL_STUDENT_H
#define DATAMODEL_STUDENT_H

#include <QObject>
#include <QString>
#include <QDate>
#include <QJsonObject>
#include <QStringList>
#include"DataModel/Person.h"

namespace DataModel {

enum class StudentStatus {
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

enum class StudentType {
    Regular = 1,
    Honors = 2,
    Weak = 3,
    Unknown = -1
};

// Utility functions
inline QString studentStatusToString(StudentStatus status) {
    switch(status) {
        case StudentStatus::Active: return QObject::tr("نشط");
        case StudentStatus::Inactive: return QObject::tr("غير نشط");
        case StudentStatus::Graduate: return QObject::tr("خريج");
        case StudentStatus::Transferred: return QObject::tr("منقول");
        case StudentStatus::Dismissed: return QObject::tr("مفصول");
        case StudentStatus::Withdrawn: return QObject::tr("منسحب");
        case StudentStatus::Failed: return QObject::tr("راسب");
        case StudentStatus::E_Learning: return QObject::tr("انتساب");
        default: return QObject::tr("غير معروف");
    }
}

// Student class
class Student : public QObject
{
    Q_OBJECT
public:
    explicit Student(QObject *parent = nullptr);
    Student(const Student& other);
    Student& operator=(const Student& other);
    Student(Student&& other) noexcept;
    Student& operator=(Student&& other) noexcept;
    ~Student() override;

    QString getFullName() const noexcept;
    bool isValid() const;

    bool isActive() const;
    bool isHonorsStudent() const;
    bool isRegularStudent() const;
    bool isWeekStudent() const;

    QString toString() const;

    // JSON
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

    // Getters
    int get_student_id() const { return student_id; }
    int get_person_id() const { return person_id; }
    QString get_first_name() const { return first_name; }
    QString get_second_name() const { return second_name; }
    QString get_third_name() const { return third_name; }
    QString get_fourth_name() const { return fourth_name; }
    QString get_student_number() const { return student_number; }
    Gender get_gender() const { return gender; }
    QDate get_date_of_birth() const { return date_of_birth; }
    int get_age() const;
    StudentStatus get_status() const { return status; }
    StudentType get_type() const { return type; }
    QDate get_enrollment_date() const { return enrollment_date; }
    QDate get_graduation_date() const { return graduation_date; }
    QString get_phone() const { return phone; }
    int get_current_class_id() const { return current_class_id; }
    QString get_current_class_name() const { return current_class_name; }
    QString get_section() const { return section; }
    int get_current_year_id() const { return current_year_id; }
    QString get_current_year_name() const { return current_year_name; }
    double get_current_average() const { return current_average; }
    int get_current_rank() const { return current_rank; }
    int get_total_absences() const { return total_absences; }

    // Setters
    bool set_student_id(int id);
    bool set_person_id(int id);
    bool set_first_name(const QString &fname);
    bool set_second_name(const QString &sname);
    bool set_third_name(const QString &tname);
    bool set_fourth_name(const QString &ftname);
    bool set_gender(QChar gender);
    bool set_date_of_birth(const QDate &dob);
    bool set_status(StudentStatus s);
    bool set_enrollment_date(const QDate &ed);
    bool set_graduation_date(const QDate &gd);
    bool set_phone(const QString &phone);
    bool set_section(const QString &sec);
    bool set_current_class_id(int id);
    bool set_current_class_name(const QString &name);
    bool set_current_year_id(int id);
    bool set_current_year_name(const QString &name);
    bool set_current_average(double avg);
    bool set_current_rank(int rank);
    bool set_student_number(const QString &number);
    

private:
    int student_id;
    int person_id;
    QString student_number;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    Gender gender;
    QDate date_of_birth;
    int age;
    StudentStatus status;
    StudentType type;
    QDate enrollment_date;
    QDate graduation_date;
    QString phone;

    int current_class_id;
    QString current_class_name;
    QString section;
    int current_year_id;
    QString current_year_name;
    double current_average;
    int current_rank;
    int total_absences;

    int calculateAge() const;
};

} // namespace DataModel

#endif // DATAMODEL_STUDENT_H
