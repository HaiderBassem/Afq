#ifndef DATAMODELS_H
#define DATAMODELS_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QTime>

namespace DataModel {

enum class Role {
    Student,
    Teacher,
    Staff,
    Admin,
    Unknown
};

enum class Status {
    Active,
    Inactive,
    Graduated,
    Transferred,
    Failed,
    Unknown
};

enum class AttendanceStatus {
    Present,
    Absent,
    Late,
    Excused,
    Unknown
};

// Helpers to map enum <-> QString
inline QString roleToString(Role r) {
    switch (r) {
    case Role::Student: return "Student";
    case Role::Teacher: return "Teacher";
    case Role::Staff:   return "Staff";
    case Role::Admin:   return "Admin";
    default: return "Unknown";
    }
}
inline Role stringToRole(const QString& s) {
    if (s == "Student") return Role::Student;
    if (s == "Teacher") return Role::Teacher;
    if (s == "Staff")   return Role::Staff;
    if (s == "Admin")   return Role::Admin;
    return Role::Unknown;
}

inline QString statusToString(Status s) {
    switch (s) {
    case Status::Active: return "Active";
    case Status::Inactive: return "Inactive";
    case Status::Graduated: return "Graduated";
    case Status::Transferred: return "Transferred";
    case Status::Failed: return "Failed";
    default: return "Unknown";
    }
}
inline Status stringToStatus(const QString& s) {
    if (s == "Active") return Status::Active;
    if (s == "Inactive") return Status::Inactive;
    if (s == "Graduated") return Status::Graduated;
    if (s == "Transferred") return Status::Transferred;
    if (s == "Failed") return Status::Failed;
    return Status::Unknown;
}

inline QString attendanceStatusToString(AttendanceStatus s) {
    switch (s) {
    case AttendanceStatus::Present: return "Present";
    case AttendanceStatus::Absent: return "Absent";
    case AttendanceStatus::Late: return "Late";
    case AttendanceStatus::Excused: return "Excused";
    default: return "Unknown";
    }
}
inline AttendanceStatus stringToAttendanceStatus(const QString& s) {
    if (s == "Present") return AttendanceStatus::Present;
    if (s == "Absent") return AttendanceStatus::Absent;
    if (s == "Late") return AttendanceStatus::Late;
    if (s == "Excused") return AttendanceStatus::Excused;
    return AttendanceStatus::Unknown;
}




// ENTITIES

struct Person {
    int person_id = 0;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    char gender = 'M';
    QDate date_of_birth;
    QString address;
    QString phone_one;
    QString phone_two;
    QString email;
    QString image_path;
    Role role = Role::Unknown;
    Status status = Status::Unknown;
    QDateTime updated_at;
};

struct School {
    int school_id = 0;
    QString name;
    QString address;
    QString phone;
    QString email;
    QDateTime created_at;
};

struct AcademicYear {
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
};

struct Term {
    int term_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    QDateTime created_at;
};

struct Class {
    int class_id = 0;
    QString name;
    QString section;
    int school_id;
    int year_id = 0;
    int capacity = 0;
    QString description;
    QDateTime created_at;
};

struct Subject {
    int subject_id = 0;
    QString name;
    QString code;
    QString description;
    int credit_hours = 0;
    QDateTime created_at;
};

struct Enrollment {
    int enrollment_id = 0;
    int person_id = 0;
    int school_id = 0;
    QString school_name;
    Role role = Role::Unknown;
    QDate start_date;
    QDate end_date;
    Status status = Status::Unknown;
    QString notes;
    QDateTime created_at;
    QDateTime updated_at;
};

struct StudentEnrollment {
    int student_enrollment_id = 0;
    int enrollment_id = 0;
    int class_id = 0;
    int year_id = 0;
    QDate start_date;
    QDate end_date;
    Status status = Status::Unknown;
    QString notes;
    QString class_name;
    QString section;
    QString full_name;
    QDateTime created_at;
    QDateTime updated_at;
};

struct TeacherEnrollment {
    int teacher_enrollment_id = 0;
    int enrollment_id = 0;
    QString specialization;
    QString qualification;
    QDate start_date;
    QDate end_date;
    Status status = Status::Unknown;
    QDateTime created_at;
    QDateTime updated_at;
};

struct CourseOffering {
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

struct Schedule {
    int schedule_id = 0;
    int offering_id = 0;
    QString day_of_week;
    QTime start_time;
    QTime end_time;
    QString classroom;
    QString schedule_type;
    QDateTime created_at;
};

struct Attendance {
    int attendance_id = 0;
    int student_enrollment_id = 0;
    int offering_id = 0;
    QDate date;
    AttendanceStatus status = AttendanceStatus::Unknown;
    QString notes;
    QDateTime recorded_at;
    QDateTime updated_at;
};

struct Grade {
    int grade_id = 0;
    int student_enrollment_id = 0;
    int offering_id = 0;
    QString exam_type;
    float score = 0.0f;
    float max_score = 0.0f;
    QDate date;
    QString notes;
    QDateTime recorded_at;
    QDateTime updated_at;

    float getPercentage() const {
        return (max_score > 0) ? (score / max_score) * 100.0f : 0.0f;
    }
};

struct Notification {
    int notification_id = 0;
    int person_id = 0;
    QString title;
    QString message;
    QString type;
    bool is_read = false;
    QDateTime created_at;
    QDateTime read_at;
};

struct SystemLog {
    int log_id = 0;
    int person_id = 0;
    QString action;
    QString table_name;
    QString old_values;
    QString new_values;
    QDateTime timestamp;
    QString ip_address;
};

// COMPLEX REPORTING STRUCTS

struct StudentSummary {
    int person_id;
    QString full_name;
    QString class_name;
    QString section;
    double overall_average = 0.0;
    double attendance_rate = 0.0;
    int total_subjects = 0;
    Status status = Status::Unknown;
};

struct ClassSummary {
    int class_id;
    QString name;
    QString section;
    int student_count = 0;
    int subject_count = 0;
    int year_id = 0;
    double class_average = 0.0;
    double attendance_rate = 0.0;
    QString main_teacher;
};

struct TeacherSummary {
    int person_id;
    QString full_name;
    int total_classes = 0;
    int total_subjects = 0;
    int total_students = 0;
    double average_class_performance = 0.0;
    int weekly_hours = 0;
};

}

#endif // DATAMODELS_H
