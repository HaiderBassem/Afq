#ifndef GRADE_H
#define GRADE_H

#include <QString>
#include <QDate>
#include <QDateTime>
#include <QJsonObject>
#include <QStringList>
#include "BaseEntity.h"


namespace DataModel
{


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

    bool isValid() const override;
    bool isPassing() const;
    double getPercentage() const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& json) override;
    QString toString() const override;
};
}
#endif // GRADE_H
