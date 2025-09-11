#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QString>
#include <QDate>
#include <QDateTime>

namespace DataModel {

struct Person
{
    int person_id = 0;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    char gender;
    QDate date_of_birth;
    QString address;
    QString phone_one;
    QString phone_two;
    QString email;
    QString image_path;
    QString role;
    QString status;
    QDateTime updated_at;
};

struct School
{
    int school_id = 0;
    QString name;
    QString address;
    QString phone;
    QString email;
    QDateTime created_at;
};

struct AcademicYear
{
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    QDateTime created_at;
};

struct Term
{
    int term_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    QDateTime created_at;
};

struct Class
{
    int class_id = 0;
    QString name;
    QString section;
    int year = 0;
    int capacity = 0;
    QString description;
    QDateTime created_at;
};

struct Subject
{
    int subject_id = 0;
    QString name;
    QString code;
    QString description;
    int credit_hours = 0;
    QDateTime created_at;
};

struct Enrollment
{
    int enrollment_id = 0;
    int person_id = 0;
    int school_id = 0;
    QString role;
    QDate start_date;
    QDate end_date;
    QString status;
    QString notes;
    QDateTime created_at;
    QDateTime updated_at;
};

struct StudentEnrollment
{
    int student_enrollment_id = 0;
    int enrollment_id = 0;
    int class_id = 0;  // Fixed typo from "calss_id"
    QDate start_date;
    QDate end_date;
    QString status;
    QString notes;
    QDateTime created_at;
    QDateTime updated_at;
};

struct TeacherEnrollment
{
    int teacher_enrollment_id = 0;
    int enrollment_id = 0;
    QString specialization;
    QString qualification;
    QDate start_date;
    QDate end_date;
    QString status;
    QDateTime created_at;
    QDateTime updated_at;
};

struct CourseOffering
{
    int offering_id = 0;
    int subject_id = 0;
    int class_id = 0;
    int teacher_enrollment_id = 0;
    int year_id = 0;
    int term_id = 0;
    QString schedule_info;
    QString classroom;
    QDateTime created_at;
};

struct Schedule
{
    int schedule_id = 0;
    int offering_id = 0;
    QString day_of_week;
    QTime start_time;
    QTime end_time;
    QString classroom;
    QString schedule_type;
    QDateTime created_at;
};

struct Attendance
{
    int attendance_id = 0;
    int student_enrollment_id = 0;
    int offering_id = 0;
    QDate date;
    QString status;  // Present, Absent, Late, Excused
    QString notes;
    QDateTime recorded_at;
    QDateTime updated_at;
};

struct Grade
{
    int grade_id = 0;
    int student_enrollment_id = 0;
    int offering_id = 0;
    QString exam_type;  // Quiz, Midterm, Final, Assignment, etc.
    float score = 0.0f;
    float max_score = 0.0f;
    QDate date;
    QString notes;
    QDateTime recorded_at;
    QDateTime updated_at;

    // Helper method to calculate percentage
    float getPercentage() const {
        return (max_score > 0) ? (score / max_score) * 100.0f : 0.0f;
    }
};

struct Notification
{
    int notification_id = 0;
    int person_id = 0;
    QString title;
    QString message;
    QString type;  // Info, Warning, Success, Error
    bool is_read = false;
    QDateTime created_at;
    QDateTime read_at;
};

struct SystemLog
{
    int log_id = 0;
    int person_id = 0;
    QString action;
    QString table_name;
    QString old_values;
    QString new_values;
    QDateTime timestamp;
    QString ip_address;
};

}

#endif // DATAMODELS_H
