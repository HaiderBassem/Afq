#ifndef SCHOOL_DATAMODELS_H
#define SCHOOL_DATAMODELS_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <QStringList>
#include <memory>
#include <optional>

namespace DataModel {

// enums

enum class Gender
{
    Male = 'M',
    Female = 'F'
};

enum class EnrollmentRole
{
    Student = 0,
    Teacher = 1,
    Staff = 2,
    Manager = 3,
    Unknown = -1
};

enum class EnrollmentStatus
{
    Inactive = 0,
    Active = 1,
    Graduate = 2,
    Transferred = 3,
    Dismissed = 4,
    Withdrawn = 5,
    Failed = 6,
    Postponed = 7,
    Unknown = -1
};

enum class StudentStatus
{
    Inactive = 0,
    Active = 1,
    Graduate = 2,
    Transferred = 3,
    Dismissed = 4,
    Withdrawn = 5,
    Failed = 6,
    Unknown = -1
};

enum class AttendanceStatus
{
    Present = 0,
    Absent = 1,
    Late = 2,
    Excused = 3,
    Vacation = 4,
    Unknown = -1
};

enum class ExamType
{
    Quiz = 0,
    Mid = 1,
    HalfYear = 2,
    Final = 3,
    SecondRound = 4,
    ThirdRound = 5,
    Unknown = -1
};

enum class DisciplinaryActionType
{
    Warning = 0,
    FirstWarning = 1,
    SecondWarning = 2,
    TemporaryDismissal = 3,
    Dismissal = 4,
    Deprivation = 5,
    Unknown = -1
};

enum class HolidayType
{
    Official = 0,
    Religious = 1,
    National = 2,
    Emergency = 3,
    Unknown = -1
};

enum class NotificationType
{
    Grades = 0,
    Attendance = 1,
    Conduct = 2,
    Unknown = -1
};

enum class NotificationPriority
{
    Normal = 0,
    Important = 1,
    VeryImportant = 2,
    Unknown = -1
};

enum class CertificateType
{
    Graduation = 0,
    Participation = 1,
    Excellence = 2,
    Thanks = 3,
    TransferBook = 4,
    Unknown = -1
};

enum class DayOfWeek
{
    Sunday = 1,
    Monday = 2,
    Tuesday = 3,
    Wednesday = 4,
    Thursday = 5,
    Unknown = -1
};

// helper functions




// NotificationType conversions
inline QString notificationTypeToString(NotificationType t)
{
    switch (t)
    {
    case NotificationType::Grades: return QObject::tr("Grades");
    case NotificationType::Attendance: return QObject::tr("Attendance");
    case NotificationType::Conduct: return QObject::tr("Conduct");
    default: return QObject::tr("Unknown");
    }
}

inline NotificationType intToNotificationType(int i)
{
    if (i >= 0 && i <= 2) return static_cast<NotificationType>(i);
    return NotificationType::Unknown;
}

// NotificationPriority conversions
inline QString notificationPriorityToString(NotificationPriority p)
{
    switch (p)
    {
    case NotificationPriority::Normal: return QObject::tr("Normal");
    case NotificationPriority::Important: return QObject::tr("Important");
    case NotificationPriority::VeryImportant: return QObject::tr("Very Important");
    default: return QObject::tr("Unknown");
    }
}

inline NotificationPriority intToNotificationPriority(int i)
{
    if (i >= 0 && i <= 2) return static_cast<NotificationPriority>(i);
    return NotificationPriority::Unknown;
}

// CertificateType conversions
inline QString certificateTypeToString(CertificateType t)
{
    switch (t) {
    case CertificateType::Graduation: return QObject::tr("Graduation");
    case CertificateType::Participation: return QObject::tr("Participation");
    case CertificateType::Excellence: return QObject::tr("Excellence");
    case CertificateType::Thanks: return QObject::tr("Thanks");
    case CertificateType::TransferBook: return QObject::tr("Transfer Book");
    default: return QObject::tr("Unknown");
    }
}

inline CertificateType intToCertificateType(int i)
{
    if (i >= 0 && i <= 4) return static_cast<CertificateType>(i);
    return CertificateType::Unknown;
}

// HolidayType conversions
inline QString holidayTypeToString(HolidayType t)
{
    switch (t) {
    case HolidayType::Official: return QObject::tr("Official");
    case HolidayType::Religious: return QObject::tr("Religious");
    case HolidayType::National: return QObject::tr("National");
    case HolidayType::Emergency: return QObject::tr("Emergency");
    default: return QObject::tr("Unknown");
    }
}

inline HolidayType intToHolidayType(int i)
{
    if (i >= 0 && i <= 3) return static_cast<HolidayType>(i);
    return HolidayType::Unknown;
}







// Gender conversions
inline QString genderToString(Gender g)
{
    switch (g)
    {
    case Gender::Male: return QObject::tr("Male");
    case Gender::Female: return QObject::tr("Female");
    default: return QObject::tr("Male");
    }
}

inline Gender charToGender(char c)
{
    if (c == 'M' || c == 'm') return Gender::Male;
    if (c == 'F' || c == 'f') return Gender::Female;
    return Gender::Male;
}

// EnrollmentRole conversions
inline QString enrollmentRoleToString(EnrollmentRole r)
{
    switch (r) {
    case EnrollmentRole::Student: return QObject::tr("Student");
    case EnrollmentRole::Teacher: return QObject::tr("Teacher");
    case EnrollmentRole::Staff: return QObject::tr("Staff");
    case EnrollmentRole::Manager: return QObject::tr("Manager");
    default: return QObject::tr("Unknown");
    }
}

inline EnrollmentRole intToEnrollmentRole(int i)
{
    if (i >= 0 && i <= 3) return static_cast<EnrollmentRole>(i);
    return EnrollmentRole::Unknown;
}

// EnrollmentStatus conversions
inline QString enrollmentStatusToString(EnrollmentStatus s)
{
    switch (s) {
    case EnrollmentStatus::Active: return QObject::tr("Active");
    case EnrollmentStatus::Inactive: return QObject::tr("Inactive");
    case EnrollmentStatus::Graduate: return QObject::tr("Graduate");
    case EnrollmentStatus::Transferred: return QObject::tr("Transferred");
    case EnrollmentStatus::Dismissed: return QObject::tr("Dismissed");
    case EnrollmentStatus::Withdrawn: return QObject::tr("Withdrawn");
    case EnrollmentStatus::Failed: return QObject::tr("Failed");
    case EnrollmentStatus::Postponed: return QObject::tr("Postponed");
    default: return QObject::tr("Unknown");
    }
}

inline EnrollmentStatus intToEnrollmentStatus(int i)
{
    if (i >= 0 && i <= 7) return static_cast<EnrollmentStatus>(i);
    return EnrollmentStatus::Unknown;
}

// AttendanceStatus conversions
inline QString attendanceStatusToString(AttendanceStatus s)
{
    switch (s) {
    case AttendanceStatus::Present: return QObject::tr("Present");
    case AttendanceStatus::Absent: return QObject::tr("Absent");
    case AttendanceStatus::Late: return QObject::tr("Late");
    case AttendanceStatus::Excused: return QObject::tr("Excused");
    case AttendanceStatus::Vacation: return QObject::tr("Vacation");
    default: return QObject::tr("Unknown");
    }
}

inline AttendanceStatus intToAttendanceStatus(int i)
{
    if (i >= 0 && i <= 4) return static_cast<AttendanceStatus>(i);
    return AttendanceStatus::Unknown;
}

// ExamType conversions
inline QString examTypeToString(ExamType t)
{
    switch (t) {
    case ExamType::Quiz: return QObject::tr("Quiz");
    case ExamType::Mid: return QObject::tr("Mid");
    case ExamType::HalfYear: return QObject::tr("Half Year Exam");
    case ExamType::Final: return QObject::tr("Final");
    case ExamType::SecondRound: return QObject::tr("Second Round");
    case ExamType::ThirdRound: return QObject::tr("Third Round");
    default: return QObject::tr("Unknown");
    }
}

inline ExamType intToExamType(int i)
{
    if (i >= 0 && i <= 5) return static_cast<ExamType>(i);
    return ExamType::Unknown;
}

// DisciplinaryActionType conversions
inline QString disciplinaryActionTypeToString(DisciplinaryActionType t)
{
    switch (t) {
    case DisciplinaryActionType::Warning: return QObject::tr("Warning");
    case DisciplinaryActionType::FirstWarning: return QObject::tr("First Warning");
    case DisciplinaryActionType::SecondWarning: return QObject::tr("Second Warning");
    case DisciplinaryActionType::TemporaryDismissal: return QObject::tr("Temporary Dismissal");
    case DisciplinaryActionType::Dismissal: return QObject::tr("Dismissal");
    case DisciplinaryActionType::Deprivation: return QObject::tr("Deprivation");
    default: return QObject::tr("Unknown");
    }
}

inline DisciplinaryActionType intToDisciplinaryActionType(int i)
{
    if (i >= 0 && i <= 5) return static_cast<DisciplinaryActionType>(i);
    return DisciplinaryActionType::Unknown;
}

// DayOfWeek conversions
inline QString dayOfWeekToString(DayOfWeek d)
{
    switch (d) {
    case DayOfWeek::Sunday: return QObject::tr("Sunday");
    case DayOfWeek::Monday: return QObject::tr("Monday");
    case DayOfWeek::Tuesday: return QObject::tr("Tuesday");
    case DayOfWeek::Wednesday: return QObject::tr("Wednesday");
    case DayOfWeek::Thursday: return QObject::tr("Thursday");
    default: return QObject::tr("Unknown");
    }
}

inline DayOfWeek intToDayOfWeek(int i)
{
    if (i >= 1 && i <= 5) return static_cast<DayOfWeek>(i);
    return DayOfWeek::Unknown;
}




// base entity




class BaseEntity
{
public:
    virtual ~BaseEntity() = default;

    // Validation
    virtual bool isValid() const { return true; }

    // JSON serialization
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject& json) = 0;

    // Debugging
    virtual QString toString() const = 0;
};




// entities

class Person : public BaseEntity
{
public:
    int person_id = 0;
    QString first_name;
    QString second_name;
    QString third_name;
    QString fourth_name;
    Gender gender = Gender::Male;
    QDate date_of_birth;
    QDateTime created_at;
    QDateTime updated_at;

    // Computed properties
    QString getFullName() const
    {
        QStringList parts;

        if (!first_name.isEmpty()) parts << first_name;
        if (!second_name.isEmpty()) parts << second_name;
        if (!third_name.isEmpty()) parts << third_name;
        if (!fourth_name.isEmpty()) parts << fourth_name;
        return parts.join(" ");
    }

    int getAge() const
    {
        if (!date_of_birth.isValid()) return 0;
        return QDate::currentDate().year() - date_of_birth.year();
    }

    bool isValid() const override
    {
        return !first_name.isEmpty() &&
               !second_name.isEmpty() &&
               !third_name.isEmpty() &&
               date_of_birth.isValid() &&
               person_id >= 0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["person_id"] = person_id;
        obj["first_name"] = first_name;
        obj["second_name"] = second_name;
        obj["third_name"] = third_name;
        obj["fourth_name"] = fourth_name;
        obj["gender"] = QString(static_cast<char>(gender));
        obj["date_of_birth"] = date_of_birth.toString(Qt::ISODate);
        obj["created_at"] = created_at.toString(Qt::ISODate);
        obj["updated_at"] = updated_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        person_id = json["person_id"].toInt();
        first_name = json["first_name"].toString();
        second_name = json["second_name"].toString();
        third_name = json["third_name"].toString();
        fourth_name = json["fourth_name"].toString();
        QString genderStr = json["gender"].toString();
        if (!genderStr.isEmpty()) {
            gender = charToGender(genderStr[0].toLatin1());
        }
        date_of_birth = QDate::fromString(json["date_of_birth"].toString(), Qt::ISODate);
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
        updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("Person[ID:%1, Name:%2, Age:%3]")
        .arg(person_id)
            .arg(getFullName())
            .arg(getAge());
    }
};

class Guardian : public BaseEntity
{
public:
    int guardian_id = 0;
    int person_id = 0;
    QString occupation;
    QString work_phone;
    QString education_level;

    // Additional fields for joins
    QString guardian_name;

    bool isValid() const override
    {
        return guardian_id >= 0 && person_id > 0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["guardian_id"] = guardian_id;
        obj["person_id"] = person_id;
        obj["occupation"] = occupation;
        obj["work_phone"] = work_phone;
        obj["education_level"] = education_level;
        obj["guardian_name"] = guardian_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        guardian_id = json["guardian_id"].toInt();
        person_id = json["person_id"].toInt();
        occupation = json["occupation"].toString();
        work_phone = json["work_phone"].toString();
        education_level = json["education_level"].toString();
        guardian_name = json["guardian_name"].toString();
    }

    QString toString() const override
    {
        return QString("Guardian[ID:%1, PersonID:%2, Occupation:%3]")
        .arg(guardian_id)
            .arg(person_id)
            .arg(occupation);
    }
};

class StudentGuardian : public BaseEntity
{
public:
    int student_guardian_id = 0;
    int student_person_id = 0;
    int guardian_id = 0;
    QString relationship;
    bool is_primary = false;

    // Additional fields for joins
    QString student_name;
    QString guardian_name;

    bool isValid() const override
    {
        return student_guardian_id >= 0 &&
               student_person_id > 0 &&
               guardian_id > 0 &&
               !relationship.isEmpty();
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["student_guardian_id"] = student_guardian_id;
        obj["student_person_id"] = student_person_id;
        obj["guardian_id"] = guardian_id;
        obj["relationship"] = relationship;
        obj["is_primary"] = is_primary;
        obj["student_name"] = student_name;
        obj["guardian_name"] = guardian_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        student_guardian_id = json["student_guardian_id"].toInt();
        student_person_id = json["student_person_id"].toInt();
        guardian_id = json["guardian_id"].toInt();
        relationship = json["relationship"].toString();
        is_primary = json["is_primary"].toBool();
        student_name = json["student_name"].toString();
        guardian_name = json["guardian_name"].toString();
    }

    QString toString() const override
    {
        return QString("StudentGuardian[ID:%1, Student:%2, Guardian:%3, Relation:%4]")
        .arg(student_guardian_id)
            .arg(student_person_id)
            .arg(guardian_id)
            .arg(relationship);
    }
};

class AcademicYear : public BaseEntity
{
public:
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    int holidays_count = 0;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               start_date.isValid() &&
               end_date.isValid() &&
               start_date < end_date;
    }

    bool isCurrentYear() const
    {
        QDate today = QDate::currentDate();
        return is_active && today >= start_date && today <= end_date;
    }

    int getDurationInDays() const
    {
        if (!start_date.isValid() || !end_date.isValid()) return 0;
        return start_date.daysTo(end_date);
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["year_id"] = year_id;
        obj["name"] = name;
        obj["start_date"] = start_date.toString(Qt::ISODate);
        obj["end_date"] = end_date.toString(Qt::ISODate);
        obj["is_active"] = is_active;
        obj["holidays_count"] = holidays_count;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        year_id = json["year_id"].toInt();
        name = json["name"].toString();
        start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
        end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
        is_active = json["is_active"].toBool();
        holidays_count = json["holidays_count"].toInt();
    }

    QString toString() const override
    {
        return QString("AcademicYear[ID:%1, Name:%2, Active:%3]")
        .arg(year_id)
            .arg(name)
            .arg(is_active ? "Yes" : "No");
    }
};

class Term : public BaseEntity
{
public:
    int term_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    bool is_active = false;
    QDate exam_start_date;
    QDate exam_end_date;

    // Additional fields for joins
    QString year_name;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               year_id > 0 &&
               start_date.isValid() &&
               end_date.isValid() &&
               start_date < end_date;
    }

    bool isCurrentTerm() const
    {
        QDate today = QDate::currentDate();
        return is_active && today >= start_date && today <= end_date;
    }

    int getWeeksCount() const
    {
        if (!start_date.isValid() || !end_date.isValid()) return 0;
        return start_date.daysTo(end_date) / 7;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["term_id"] = term_id;
        obj["year_id"] = year_id;
        obj["name"] = name;
        obj["start_date"] = start_date.toString(Qt::ISODate);
        obj["end_date"] = end_date.toString(Qt::ISODate);
        obj["is_active"] = is_active;
        obj["exam_start_date"] = exam_start_date.toString(Qt::ISODate);
        obj["exam_end_date"] = exam_end_date.toString(Qt::ISODate);
        obj["year_name"] = year_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        term_id = json["term_id"].toInt();
        year_id = json["year_id"].toInt();
        name = json["name"].toString();
        start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
        end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
        is_active = json["is_active"].toBool();
        exam_start_date = QDate::fromString(json["exam_start_date"].toString(), Qt::ISODate);
        exam_end_date = QDate::fromString(json["exam_end_date"].toString(), Qt::ISODate);
        year_name = json["year_name"].toString();
    }

    QString toString() const override {
        return QString("Term[ID:%1, Name:%2, Year:%3, Active:%4]")
        .arg(term_id)
            .arg(name)
            .arg(year_id)
            .arg(is_active ? "Yes" : "No");
    }
};

class EducationStage : public BaseEntity
{
public:
    int stage_id = 0;
    QString name;
    int min_grade = 0;
    int max_grade = 0;
    bool has_ministerial_exam = false;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               min_grade > 0 &&
               max_grade >= min_grade &&
               max_grade <= 12;
    }

    bool containsGrade(int grade) const
    {
        return grade >= min_grade && grade <= max_grade;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["stage_id"] = stage_id;
        obj["name"] = name;
        obj["min_grade"] = min_grade;
        obj["max_grade"] = max_grade;
        obj["has_ministerial_exam"] = has_ministerial_exam;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        stage_id = json["stage_id"].toInt();
        name = json["name"].toString();
        min_grade = json["min_grade"].toInt();
        max_grade = json["max_grade"].toInt();
        has_ministerial_exam = json["has_ministerial_exam"].toBool();
    }

    QString toString() const override
    {
        return QString("EducationStage[ID:%1, Name:%2, Grades:%3-%4]")
        .arg(stage_id)
            .arg(name)
            .arg(min_grade)
            .arg(max_grade);
    }
};

class Class : public BaseEntity
{
public:
    int class_id = 0;
    int stage_id = 0;
    int grade_level = 1;
    QString name;
    QString section;
    int year = 0;
    bool has_ministerial_exam = false;

    // Additional fields for joins
    QString stage_name;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               stage_id > 0 &&
               year > 2000 &&
               grade_level >= 1 && grade_level <= 12;
    }

    QString getFullClassName() const
    {
        if (section.isEmpty()) return name;
        return QString("%1 - %2").arg(name).arg(section);
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["class_id"] = class_id;
        obj["stage_id"] = stage_id;
        obj["grade_level"] = grade_level;
        obj["name"] = name;
        obj["section"] = section;
        obj["year"] = year;
        obj["has_ministerial_exam"] = has_ministerial_exam;
        obj["stage_name"] = stage_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        class_id = json["class_id"].toInt();
        stage_id = json["stage_id"].toInt();
        grade_level = json["grade_level"].toInt();
        name = json["name"].toString();
        section = json["section"].toString();
        year = json["year"].toInt();
        has_ministerial_exam = json["has_ministerial_exam"].toBool();
        stage_name = json["stage_name"].toString();
    }

    QString toString() const override
    {
        return QString("Class[ID:%1, Name:%2, Section:%3, Grade:%4]")
        .arg(class_id)
            .arg(name)
            .arg(section)
            .arg(grade_level);
    }
};

class Enrollment : public BaseEntity
{
public:
    int enrollment_id = 0;
    int person_id = 0;
    QString enrollment_number;
    EnrollmentRole role = EnrollmentRole::Unknown;
    QDate start_date;
    QDate end_date;
    EnrollmentStatus status = EnrollmentStatus::Unknown;
    QDateTime created_at;
    QDateTime updated_at;

    // Additional fields for joins
    QString person_name;

    bool isValid() const override
    {
        return person_id > 0 &&
               role != EnrollmentRole::Unknown &&
               start_date.isValid();
    }

    bool isActive() const
    {
        return status == EnrollmentStatus::Active &&
               (!end_date.isValid() || end_date >= QDate::currentDate());
    }

    int getDurationInDays() const
    {
        if (!start_date.isValid()) return 0;
        QDate endDate = end_date.isValid() ? end_date : QDate::currentDate();
        return start_date.daysTo(endDate);
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["enrollment_id"] = enrollment_id;
        obj["person_id"] = person_id;
        obj["enrollment_number"] = enrollment_number;
        obj["role"] = static_cast<int>(role);
        obj["start_date"] = start_date.toString(Qt::ISODate);
        obj["end_date"] = end_date.toString(Qt::ISODate);
        obj["status"] = static_cast<int>(status);
        obj["person_name"] = person_name;
        obj["created_at"] = created_at.toString(Qt::ISODate);
        obj["updated_at"] = updated_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        enrollment_id = json["enrollment_id"].toInt();
        person_id = json["person_id"].toInt();
        enrollment_number = json["enrollment_number"].toString();
        role = intToEnrollmentRole(json["role"].toInt());
        start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
        end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
        status = intToEnrollmentStatus(json["status"].toInt());
        person_name = json["person_name"].toString();
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
        updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("Enrollment[ID:%1, Person:%2, Role:%3, Status:%4]")
        .arg(enrollment_id)
            .arg(person_id)
            .arg(enrollmentRoleToString(role))
            .arg(enrollmentStatusToString(status));
    }
};

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

    // Additional fields for joins
    QString student_name;
    QString class_name;
    QString section;
    QString year_name;

    bool isValid() const override
    {
        return enrollment_id > 0 &&
               class_id > 0 &&
               year_id > 0 &&
               start_date.isValid();
    }

    bool shouldPromote() const
    {
        return status == StudentStatus::Active && !is_repeated && final_average >= 50.0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["student_enrollment_id"] = student_enrollment_id;
        obj["enrollment_id"] = enrollment_id;
        obj["class_id"] = class_id;
        obj["year_id"] = year_id;
        obj["start_date"] = start_date.toString(Qt::ISODate);
        obj["end_date"] = end_date.toString(Qt::ISODate);
        obj["status"] = static_cast<int>(status);
        obj["repeat_count"] = repeat_count;
        obj["is_graduated"] = is_graduated;
        obj["is_repeated"] = is_repeated;
        obj["final_average"] = final_average;
        obj["rank_in_class"] = rank_in_class;
        obj["is_ministerial_exam"] = is_ministerial_exam;
        obj["is_eligible_for_exam"] = is_eligible_for_exam;
        obj["student_name"] = student_name;
        obj["class_name"] = class_name;
        obj["section"] = section;
        obj["year_name"] = year_name;
        obj["created_at"] = created_at.toString(Qt::ISODate);
        obj["updated_at"] = updated_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        student_enrollment_id = json["student_enrollment_id"].toInt();
        enrollment_id = json["enrollment_id"].toInt();
        class_id = json["class_id"].toInt();
        year_id = json["year_id"].toInt();
        start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
        end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
        status = static_cast<StudentStatus>(json["status"].toInt());
        repeat_count = json["repeat_count"].toInt();
        is_graduated = json["is_graduated"].toBool();
        is_repeated = json["is_repeated"].toBool();
        final_average = json["final_average"].toDouble();
        rank_in_class = json["rank_in_class"].toInt();
        is_ministerial_exam = json["is_ministerial_exam"].toBool();
        is_eligible_for_exam = json["is_eligible_for_exam"].toBool();
        student_name = json["student_name"].toString();
        class_name = json["class_name"].toString();
        section = json["section"].toString();
        year_name = json["year_name"].toString();
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
        updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("StudentEnrollment[ID:%1, Student:%2, Class:%3, Status:%4]")
        .arg(student_enrollment_id)
            .arg(enrollment_id)
            .arg(class_id)
            .arg(static_cast<int>(status));
    }
};

class Subject : public BaseEntity
{
public:
    int subject_id = 0;
    QString name;
    QString code;
    double passing_grade = 50.0;
    int stage_id = 0;

    // Additional fields for joins
    QString stage_name;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               passing_grade >= 0 && passing_grade <= 100;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["subject_id"] = subject_id;
        obj["name"] = name;
        obj["code"] = code;
        obj["passing_grade"] = passing_grade;
        obj["stage_id"] = stage_id;
        obj["stage_name"] = stage_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        subject_id = json["subject_id"].toInt();
        name = json["name"].toString();
        code = json["code"].toString();
        passing_grade = json["passing_grade"].toDouble();
        stage_id = json["stage_id"].toInt();
        stage_name = json["stage_name"].toString();
    }

    QString toString() const override
    {
        return QString("Subject[ID:%1, Name:%2, Code:%3, PassingGrade:%4]")
        .arg(subject_id)
            .arg(name)
            .arg(code)
            .arg(passing_grade);
    }
};

class Curriculum : public BaseEntity
{
public:
    int curriculum_id = 0;
    int class_id = 0;
    int subject_id = 0;
    int year_id = 0;
    QString textbook_name;

    // Additional fields for joins
    QString class_name;
    QString subject_name;
    QString year_name;

    bool isValid() const override
    {
        return class_id > 0 &&
               subject_id > 0 &&
               year_id > 0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["curriculum_id"] = curriculum_id;
        obj["class_id"] = class_id;
        obj["subject_id"] = subject_id;
        obj["year_id"] = year_id;
        obj["textbook_name"] = textbook_name;
        obj["class_name"] = class_name;
        obj["subject_name"] = subject_name;
        obj["year_name"] = year_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        curriculum_id = json["curriculum_id"].toInt();
        class_id = json["class_id"].toInt();
        subject_id = json["subject_id"].toInt();
        year_id = json["year_id"].toInt();
        textbook_name = json["textbook_name"].toString();
        class_name = json["class_name"].toString();
        subject_name = json["subject_name"].toString();
        year_name = json["year_name"].toString();
    }

    QString toString() const override
    {
        return QString("Curriculum[ID:%1, Class:%2, Subject:%3, Year:%4]")
        .arg(curriculum_id)
            .arg(class_id)
            .arg(subject_id)
            .arg(year_id);
    }
};

class CourseOffering : public BaseEntity
{
public:
    int offering_id = 0;
    int curriculum_id = 0;
    int teacher_enrollment_id = 0;
    int term_id = 0;
    int current_students = 0;

    // Additional fields for joins
    QString subject_name;
    QString teacher_name;
    QString class_name;
    QString term_name;

    bool isValid() const override
    {
        return curriculum_id > 0 &&
               teacher_enrollment_id > 0 &&
               term_id > 0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["offering_id"] = offering_id;
        obj["curriculum_id"] = curriculum_id;
        obj["teacher_enrollment_id"] = teacher_enrollment_id;
        obj["term_id"] = term_id;
        obj["current_students"] = current_students;
        obj["subject_name"] = subject_name;
        obj["teacher_name"] = teacher_name;
        obj["class_name"] = class_name;
        obj["term_name"] = term_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        offering_id = json["offering_id"].toInt();
        curriculum_id = json["curriculum_id"].toInt();
        teacher_enrollment_id = json["teacher_enrollment_id"].toInt();
        term_id = json["term_id"].toInt();
        current_students = json["current_students"].toInt();
        subject_name = json["subject_name"].toString();
        teacher_name = json["teacher_name"].toString();
        class_name = json["class_name"].toString();
        term_name = json["term_name"].toString();
    }

    QString toString() const override
    {
        return QString("CourseOffering[ID:%1, Curriculum:%2, Teacher:%3, Students:%4]")
        .arg(offering_id)
            .arg(curriculum_id)
            .arg(teacher_enrollment_id)
            .arg(current_students);
    }
};

class Schedule : public BaseEntity
{
public:
    int schedule_id = 0;
    int offering_id = 0;
    DayOfWeek day_of_week = DayOfWeek::Sunday;
    int period_number = 1;
    QTime start_time;
    QTime end_time;
    QString classroom;

    // Additional fields for joins
    QString subject_name;
    QString teacher_name;
    QString class_name;

    bool isValid() const override
    {
        return offering_id > 0 &&
               period_number >= 1 && period_number <= 6 &&
               start_time.isValid() &&
               end_time.isValid() &&
               start_time < end_time &&
               !classroom.isEmpty();
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["schedule_id"] = schedule_id;
        obj["offering_id"] = offering_id;
        obj["day_of_week"] = static_cast<int>(day_of_week);
        obj["period_number"] = period_number;
        obj["start_time"] = start_time.toString(Qt::ISODate);
        obj["end_time"] = end_time.toString(Qt::ISODate);
        obj["classroom"] = classroom;
        obj["subject_name"] = subject_name;
        obj["teacher_name"] = teacher_name;
        obj["class_name"] = class_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        schedule_id = json["schedule_id"].toInt();
        offering_id = json["offering_id"].toInt();
        day_of_week = intToDayOfWeek(json["day_of_week"].toInt());
        period_number = json["period_number"].toInt();
        start_time = QTime::fromString(json["start_time"].toString(), Qt::ISODate);
        end_time = QTime::fromString(json["end_time"].toString(), Qt::ISODate);
        classroom = json["classroom"].toString();
        subject_name = json["subject_name"].toString();
        teacher_name = json["teacher_name"].toString();
        class_name = json["class_name"].toString();
    }

    QString toString() const override
    {
        return QString("Schedule[ID:%1, Day:%2, Period:%3, Classroom:%4]")
        .arg(schedule_id)
            .arg(dayOfWeekToString(day_of_week))
            .arg(period_number)
            .arg(classroom);
    }
};

struct Attendance : public BaseEntity
{
    int attendance_id = 0;
    int student_enrollment_id = 0;
    QDate date;
    AttendanceStatus status = AttendanceStatus::Unknown;
    QStringList period_absent;
    QString reason;
    bool excuse_document = false;
    int created_by = 0;
    QDateTime created_at;

    // Additional fields for joins
    QString student_name;
    QString class_name;
    QString creator_name;

    bool isValid() const override
    {
        return student_enrollment_id > 0 &&
               date.isValid();
    }

    bool isExcused() const
    {
        return status == AttendanceStatus::Excused || excuse_document;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["attendance_id"] = attendance_id;
        obj["student_enrollment_id"] = student_enrollment_id;
        obj["date"] = date.toString(Qt::ISODate);
        obj["status"] = static_cast<int>(status);
        QJsonArray periodsArray;
        for (const QString& period : period_absent)
        {
            periodsArray.append(period);
        }
        obj["period_absent"] = periodsArray;
        obj["reason"] = reason;
        obj["excuse_document"] = excuse_document;
        obj["created_by"] = created_by;
        obj["student_name"] = student_name;
        obj["class_name"] = class_name;
        obj["creator_name"] = creator_name;
        obj["created_at"] = created_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        attendance_id = json["attendance_id"].toInt();
        student_enrollment_id = json["student_enrollment_id"].toInt();
        date = QDate::fromString(json["date"].toString(), Qt::ISODate);
        status = intToAttendanceStatus(json["status"].toInt());
        period_absent.clear();
        QJsonArray periodsArray = json["period_absent"].toArray();
        for (const QJsonValue& value : periodsArray)
        {
            period_absent.append(value.toString());
        }
        reason = json["reason"].toString();
        excuse_document = json["excuse_document"].toBool();
        created_by = json["created_by"].toInt();
        student_name = json["student_name"].toString();
        class_name = json["class_name"].toString();
        creator_name = json["creator_name"].toString();
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("Attendance[ID:%1, Student:%2, Date:%3, Status:%4]")
        .arg(attendance_id)
            .arg(student_enrollment_id)
            .arg(date.toString("yyyy-MM-dd"))
            .arg(attendanceStatusToString(status));
    }
};

class Grade : public BaseEntity
{
public:
    int grade_id = 0;
    int student_enrollment_id = 0;
    int offering_id = 0;
    ExamType exam_type = ExamType::Unknown;
    double score = 0.0;
    double max_score = 100.0;
    double weight = 1.0;
    QDate exam_date;
    bool is_absent = false;
    QString notes;
    int graded_by = 0;
    QDateTime graded_at;

    // Additional fields for joins
    QString student_name;
    QString subject_name;
    QString grader_name;
    QString class_name;

    bool isValid() const override
    {
        return student_enrollment_id > 0 &&
               offering_id > 0 &&
               score >= 0 && score <= max_score &&
               exam_date.isValid();
    }

    bool isPassing() const
    {
        return !is_absent && score >= 50.0; // Assuming 50 is passing grade
    }

    double getPercentage() const
    {
        if (max_score == 0) return 0.0;
        return (score / max_score) * 100.0;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["grade_id"] = grade_id;
        obj["student_enrollment_id"] = student_enrollment_id;
        obj["offering_id"] = offering_id;
        obj["exam_type"] = static_cast<int>(exam_type);
        obj["score"] = score;
        obj["max_score"] = max_score;
        obj["weight"] = weight;
        obj["exam_date"] = exam_date.toString(Qt::ISODate);
        obj["is_absent"] = is_absent;
        obj["notes"] = notes;
        obj["graded_by"] = graded_by;
        obj["student_name"] = student_name;
        obj["subject_name"] = subject_name;
        obj["grader_name"] = grader_name;
        obj["class_name"] = class_name;
        obj["graded_at"] = graded_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        grade_id = json["grade_id"].toInt();
        student_enrollment_id = json["student_enrollment_id"].toInt();
        offering_id = json["offering_id"].toInt();
        exam_type = intToExamType(json["exam_type"].toInt());
        score = json["score"].toDouble();
        max_score = json["max_score"].toDouble();
        weight = json["weight"].toDouble();
        exam_date = QDate::fromString(json["exam_date"].toString(), Qt::ISODate);
        is_absent = json["is_absent"].toBool();
        notes = json["notes"].toString();
        graded_by = json["graded_by"].toInt();
        student_name = json["student_name"].toString();
        subject_name = json["subject_name"].toString();
        grader_name = json["grader_name"].toString();
        class_name = json["class_name"].toString();
        graded_at = QDateTime::fromString(json["graded_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("Grade[ID:%1, Student:%2, Subject:%3, Score:%4/%5]")
        .arg(grade_id)
            .arg(student_enrollment_id)
            .arg(offering_id)
            .arg(score)
            .arg(max_score);
    }
};

class DisciplinaryAction : public BaseEntity
{
public:
    int action_id = 0;
    int student_enrollment_id = 0;
    DisciplinaryActionType action_type = DisciplinaryActionType::Unknown;
    QString reason;
    QDate action_date;
    int duration_days = 0;
    int issued_by = 0;
    QString notes;
    bool parent_notified = false;
    QDateTime created_at;

    // Additional fields for joins
    QString student_name;
    QString issuer_name;
    QString class_name;

    bool isValid() const override
    {
        return student_enrollment_id > 0 &&
               action_type != DisciplinaryActionType::Unknown &&
               !reason.isEmpty() &&
               action_date.isValid();
    }

    bool isActive() const
    {
        if (duration_days <= 0) return false;
        return action_date.daysTo(QDate::currentDate()) < duration_days;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["action_id"] = action_id;
        obj["student_enrollment_id"] = student_enrollment_id;
        obj["action_type"] = static_cast<int>(action_type);
        obj["reason"] = reason;
        obj["action_date"] = action_date.toString(Qt::ISODate);
        obj["duration_days"] = duration_days;
        obj["issued_by"] = issued_by;
        obj["notes"] = notes;
        obj["parent_notified"] = parent_notified;
        obj["student_name"] = student_name;
        obj["issuer_name"] = issuer_name;
        obj["class_name"] = class_name;
        obj["created_at"] = created_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        action_id = json["action_id"].toInt();
        student_enrollment_id = json["student_enrollment_id"].toInt();
        action_type = intToDisciplinaryActionType(json["action_type"].toInt());
        reason = json["reason"].toString();
        action_date = QDate::fromString(json["action_date"].toString(), Qt::ISODate);
        duration_days = json["duration_days"].toInt();
        issued_by = json["issued_by"].toInt();
        notes = json["notes"].toString();
        parent_notified = json["parent_notified"].toBool();
        student_name = json["student_name"].toString();
        issuer_name = json["issuer_name"].toString();
        class_name = json["class_name"].toString();
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("DisciplinaryAction[ID:%1, Student:%2, Type:%3, Date:%4]")
        .arg(action_id)
            .arg(student_enrollment_id)
            .arg(disciplinaryActionTypeToString(action_type))
            .arg(action_date.toString("yyyy-MM-dd"));
    }
};

class Holiday : public BaseEntity
{
public:
    int holiday_id = 0;
    int year_id = 0;
    QString name;
    QDate start_date;
    QDate end_date;
    HolidayType type = HolidayType::Unknown;
    bool is_planned = true;

    // Additional fields for joins
    QString year_name;

    bool isValid() const override
    {
        return !name.isEmpty() &&
               year_id > 0 &&
               start_date.isValid() &&
               end_date.isValid() &&
               start_date <= end_date;
    }

    bool isCurrentHoliday() const
    {
        QDate today = QDate::currentDate();
        return today >= start_date && today <= end_date;
    }

    int getDurationInDays() const
    {
        if (!start_date.isValid() || !end_date.isValid()) return 0;
        return start_date.daysTo(end_date) + 1;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["holiday_id"] = holiday_id;
        obj["year_id"] = year_id;
        obj["name"] = name;
        obj["start_date"] = start_date.toString(Qt::ISODate);
        obj["end_date"] = end_date.toString(Qt::ISODate);
        obj["type"] = static_cast<int>(type);
        obj["is_planned"] = is_planned;
        obj["year_name"] = year_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        holiday_id = json["holiday_id"].toInt();
        year_id = json["year_id"].toInt();
        name = json["name"].toString();
        start_date = QDate::fromString(json["start_date"].toString(), Qt::ISODate);
        end_date = QDate::fromString(json["end_date"].toString(), Qt::ISODate);
        type = static_cast<HolidayType>(json["type"].toInt());
        is_planned = json["is_planned"].toBool();
        year_name = json["year_name"].toString();
    }

    QString toString() const override
    {
        return QString("Holiday[ID:%1, Name:%2, Duration:%3 days]")
        .arg(holiday_id)
            .arg(name)
            .arg(getDurationInDays());
    }
};

class Notification : public BaseEntity
{
public:
    int notification_id = 0;
    QString title;
    QString content;
    NotificationType type = NotificationType::Unknown;
    NotificationPriority priority = NotificationPriority::Normal;
    int sender_id = 0;
    QDateTime created_at;

    // Additional fields for joins
    QString sender_name;

    bool isValid() const override
    {
        return !title.isEmpty() &&
               !content.isEmpty();
    }

    bool isImportant() const
    {
        return priority == NotificationPriority::Important ||
               priority == NotificationPriority::VeryImportant;
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["notification_id"] = notification_id;
        obj["title"] = title;
        obj["content"] = content;
        obj["type"] = static_cast<int>(type);
        obj["priority"] = static_cast<int>(priority);
        obj["sender_id"] = sender_id;
        obj["sender_name"] = sender_name;
        obj["created_at"] = created_at.toString(Qt::ISODate);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        notification_id = json["notification_id"].toInt();
        title = json["title"].toString();
        content = json["content"].toString();
        type = static_cast<NotificationType>(json["type"].toInt());
        priority = static_cast<NotificationPriority>(json["priority"].toInt());
        sender_id = json["sender_id"].toInt();
        sender_name = json["sender_name"].toString();
        created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    }

    QString toString() const override
    {
        return QString("Notification[ID:%1, Title:%2, Priority:%3]")
        .arg(notification_id)
            .arg(title)
            .arg(static_cast<int>(priority));
    }
};

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

    bool isValid() const override
    {
        return student_enrollment_id > 0 &&
               certificate_type != CertificateType::Unknown &&
               issue_date.isValid() &&
               !certificate_number.isEmpty();
    }

    QJsonObject toJson() const override
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

    void fromJson(const QJsonObject& json) override
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

    QString toString() const override
    {
        return QString("Certificate[ID:%1, Student:%2, Type:%3, Number:%4]")
        .arg(certificate_id)
            .arg(student_enrollment_id)
            .arg(static_cast<int>(certificate_type))
            .arg(certificate_number);
    }
};

// UTILITY STRUCTURES

class StudentSummary : public BaseEntity
{
public:
    int student_enrollment_id = 0;
    QString full_name;
    QString class_name;
    QString section;
    double final_average = 0.0;
    int rank_in_class = 0;
    int total_absences = 0;
    int disciplinary_actions = 0;
    StudentStatus status = StudentStatus::Unknown;

    bool isValid() const override
    {
        return student_enrollment_id > 0 && !full_name.isEmpty();
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["student_enrollment_id"] = student_enrollment_id;
        obj["full_name"] = full_name;
        obj["class_name"] = class_name;
        obj["section"] = section;
        obj["final_average"] = final_average;
        obj["rank_in_class"] = rank_in_class;
        obj["total_absences"] = total_absences;
        obj["disciplinary_actions"] = disciplinary_actions;
        obj["status"] = static_cast<int>(status);
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        student_enrollment_id = json["student_enrollment_id"].toInt();
        full_name = json["full_name"].toString();
        class_name = json["class_name"].toString();
        section = json["section"].toString();
        final_average = json["final_average"].toDouble();
        rank_in_class = json["rank_in_class"].toInt();
        total_absences = json["total_absences"].toInt();
        disciplinary_actions = json["disciplinary_actions"].toInt();
        status = static_cast<StudentStatus>(json["status"].toInt());
    }

    QString toString() const override
    {
        return QString("StudentSummary[ID:%1, Name:%2, Average:%3, Rank:%4]")
        .arg(student_enrollment_id)
            .arg(full_name)
            .arg(final_average)
            .arg(rank_in_class);
    }
};

class ClassSummary : public BaseEntity
{
public:
    int class_id = 0;
    QString class_name;
    QString section;
    int total_students = 0;
    int active_students = 0;
    double class_average = 0.0;
    int subjects_count = 0;
    QString teacher_name;

    bool isValid() const override
    {
        return class_id > 0 && !class_name.isEmpty();
    }

    QJsonObject toJson() const override
    {
        QJsonObject obj;
        obj["class_id"] = class_id;
        obj["class_name"] = class_name;
        obj["section"] = section;
        obj["total_students"] = total_students;
        obj["active_students"] = active_students;
        obj["class_average"] = class_average;
        obj["subjects_count"] = subjects_count;
        obj["teacher_name"] = teacher_name;
        return obj;
    }

    void fromJson(const QJsonObject& json) override
    {
        class_id = json["class_id"].toInt();
        class_name = json["class_name"].toString();
        section = json["section"].toString();
        total_students = json["total_students"].toInt();
        active_students = json["active_students"].toInt();
        class_average = json["class_average"].toDouble();
        subjects_count = json["subjects_count"].toInt();
        teacher_name = json["teacher_name"].toString();
    }

    QString toString() const override
    {
        return QString("ClassSummary[ID:%1, Name:%2, Students:%3, Average:%4]")
        .arg(class_id)
            .arg(class_name)
            .arg(active_students)
            .arg(class_average);
    }
};

} // namespace SchoolDataModel

#endif // SCHOOL_DATAMODELS_H
