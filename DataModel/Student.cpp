#include "Student.h"

DataModel::Student::Student(QObject *parent)
    : QObject(parent),
    student_id(0),
    person_id(0),
    student_number(""),
    first_name(""),
    second_name(""),
    third_name(""),
    fourth_name(""),
    gender(DataModel::Gender::Male),
    status(DataModel::StudentStatus::Inactive),
    type(DataModel::StudentType::Regular),
    age(0),
    current_class_id(0),
    current_year_id(0),
    current_average(0.0),
    current_rank(0),
    total_absences(0)
{
}

//I think I don't need it ... but I will leave it here ..
QString DataModel::Student::getFullName() const noexcept
{
    QStringList parts;
    if (!this->first_name.isEmpty()) parts << this->first_name;
    if (!this->second_name.isEmpty()) parts << this->second_name;
    if (!this->third_name.isEmpty()) parts << this->third_name;
    if (!this->fourth_name.isEmpty()) parts << this->fourth_name;
    return parts.join(" ");
}

bool DataModel::Student::isValid() const
{
    return this->student_id > 0 &&
           this->person_id > 0 &&
           !this->first_name.isEmpty() &&
           !this->second_name.isEmpty() &&
           this->date_of_birth.isValid() &&
           !this->student_number.isEmpty();
}



bool DataModel::Student::isActive() const
{
    return this->status == StudentStatus::Active;
}

bool DataModel::Student::isHonorsStudent() const
{
    return this->type == StudentType::Honors;
}

bool DataModel::Student::isRegularStudent() const
{
    return this->type == StudentType::Regular;
}

bool DataModel::Student::isWeekStudent() const
{
    return this->type == StudentType::Weak;
}

bool DataModel::Student::isSpecialNeedsStudent() const
{
    return this->type == StudentType::SpecialNeeds;
}

int DataModel::Student::calculateAge() const
{
    if(!this->date_of_birth.isValid()) return 0;
    return QDate::currentDate().year() - this->date_of_birth.year();
}


QString DataModel::Student::toString() const
{
    return QString("Student[ID:%1, Number:%2, Name:%3, Status:%4, Class:%5]")
        .arg(this->student_id)
        .arg(this->student_number)
        .arg(this->getFullName())
        .arg(studentStatusToString(this->status))
        .arg(this->current_class_name);
}

QJsonObject DataModel::Student::toJson() const
{
    QJsonObject obj;
    obj["student_id"] = this->student_id;
    obj["person_id"] = this->person_id;
    obj["first_name"] = this->first_name;
    obj["second_name"] = this->second_name;
    obj["third_name"] = this->third_name;
    obj["fourth_name"] = this->fourth_name;
    obj["full_name"] = this->getFullName();
    obj["gender"] = DataModel::genderToString(this->gender);
    obj["date_of_birth"] = this->date_of_birth.toString("yyyy-MM-dd");
    obj["age"] = this->calculateAge();
    obj["student_number"] = this->student_number;
    obj["status"] = static_cast<int>(this->status);
    obj["type"] = static_cast<int>(this->type);
    obj["enrollment_date"] = this->enrollment_date.toString(Qt::ISODate);
    obj["graduation_date"] = this->graduation_date.toString(Qt::ISODate);
    obj["current_class_id"] = this->current_class_id;
    obj["current_class_name"] = this->current_class_name;
    obj["section"] = this->section;
    obj["current_year_id"] = this->current_year_id;
    obj["current_year_name"] = this->current_year_name;
    obj["current_average"] = this->current_average;
    obj["current_rank"] = this->current_rank;
    obj["phone"] = this->phone;
    obj["total_absences"] = this->total_absences;

    return obj;
}

void DataModel::Student::fromJson(const QJsonObject &json)
{
    this->student_id = json.value("student_id").toInt(0);
    this->person_id = json.value("person_id").toInt(0);

    this->first_name  = json.value("first_name").toString();
    this->second_name = json.value("second_name").toString();
    this->third_name  = json.value("third_name").toString();
    this->fourth_name = json.value("fourth_name").toString();

    QString genderStr = json.value("gender").toString().trimmed();
    this->gender = genderStr.isEmpty() ? Gender::Male
                                 : DataModel::charToGender(genderStr[0]);

    this->date_of_birth = QDate::fromString(json.value("date_of_birth").toString(), Qt::ISODate);
    this->student_number = json.value("student_number").toString();

    this->status = DataModel::intToStudentStatus(json.value("status").toInt(0));
    this->type   = static_cast<DataModel::StudentType>(json.value("type").toInt(0));

    this->enrollment_date  = QDate::fromString(json.value("enrollment_date").toString(), Qt::ISODate);
    this->graduation_date  = QDate::fromString(json.value("graduation_date").toString(), Qt::ISODate);

    this->current_class_id   = json.value("current_class_id").toInt(0);
    this->current_class_name = json.value("current_class_name").toString();
    this->section            = json.value("section").toString();

    this->current_year_id   = json.value("current_year_id").toInt(0);
    this->current_year_name = json.value("current_year_name").toString();

    this->current_average = json.value("current_average").toDouble(0.0);
    this->current_rank    = json.value("current_rank").toInt(0);
    this->total_absences  = json.value("total_absences").toInt(0);
}




































// Student summary class








DataModel::StudentSummary::StudentSummary(QObject *parent)
    : QObject(parent),
    student_id(0),
    student_number(""),
    first_name(""),
    second_name(""),
    third_name(""),
    fourth_name(""),
    rank(0),
    attendance_rate(0.0),
    completed_subjects(0),
    total_subjects(0),
    total_absences(0),
    average(0.0)
{
}


bool DataModel::StudentSummary::isValid() const
{
    return this->student_id > 0 && !this->first_name.trimmed().isEmpty() && !this->second_name.trimmed().isEmpty();
}

double DataModel::StudentSummary::getCompletionRate() const
{
    if(this->total_subjects == 0) return 0.0;
    return (static_cast<double>(this->completed_subjects) / this->total_subjects) * 100.0;
}

QJsonObject DataModel::StudentSummary::toJson() const
{
    QJsonObject obj;
    obj["student_id"] = this->student_id;
    obj["student_number"] = this->student_number;
    obj["first_name"] = this->first_name;
    obj["second_name"] = this->second_name;
    obj["third_name"] = this->third_name;
    obj["fourth_name"] = this->fourth_name;
    obj["class_name"] = this->class_name;\
    obj["section"] = this->section;
    obj["status"] = this->status;
    obj["average"] = this->average;
    obj["rank"] = this->rank;
    obj["attendance_rate"] = this->attendance_rate;
    obj["completed_subjects"] = this->completed_subjects;
    obj["total_subjects"] = this->total_subjects;
    obj["total_absences"] = this->total_absences;
    obj["completion_rate"] = this->getCompletionRate();
    return obj;

}

void DataModel::StudentSummary::fromJson(const QJsonObject &json)
{
    this->student_id = json["student_id"].toInt();
    this->student_number = json["student_number"].toString();
    this->first_name  = json.value("first_name").toString();
    this->second_name = json.value("second_name").toString();
    this->third_name  = json.value("third_name").toString();
    this->fourth_name = json.value("fourth_name").toString();
    this->class_name = json["class_name"].toString();
    this->section = json["section"].toString();
    this->status = json["status"].toString();
    this->average = json["average"].toDouble();
    this->rank = json["rank"].toInt();
    this->attendance_rate = json["attendance_rate"].toInt();
    this->completed_subjects = json["completed_subjects"].toInt();
    this->total_subjects = json["total_subjects"].toInt();
    this->total_absences = json["total_absences"].toInt();
}

QString DataModel::StudentSummary::getFullName() const noexcept
{
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}






































