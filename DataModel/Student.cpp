#include "Student.h"

Student::Student(QObject *parent)
    : QObject{parent}
{}

QString DataModel::Student::getFullName() const
{
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}

bool DataModel::Student::isActive() const
{
    return status == StudentStatus::Active;
}

bool DataModel::Student::isHonorsStudent() const
{
    return type == StudentType::Honors;
}

bool DataModel::Student::isRegularStudent() const
{
    return type == StudentType::Regular;
}

bool DataModel::Student::isWeekStudent() const
{
    return type == StudentType::Week;
}

bool DataModel::Student::isSpecialNeedsStudent() const
{
    return type == StudentType::SpecialNeeds;
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
    obj["gender"] = this->gender;
    obj["date_of_birth"] = this->date_of_birth;
    obj["age"] = this->age;
    obj["student_number"] = this->student_number;
    obj["status"] = static_cast<int>(this->status);
    obj["type"] = static_cast<int>(this->type);
    obj["enrollment_date"] = this->enrollment_date.toString(Qt::ISODate);
    obj["graduation_date"] = this->graduation_date.toString(Qt::ISODate);
    obj["current_class_id"] = this->current_class_id;
    obj["current_class_name"] = this->current_class_name;
    obj["current_year_id"] = this->current_year_id;
    obj["current_year_name"] = this->current_year_name;
    obj["current_average"] = this->current_average;
    obj["phone"] = this->phone;

    return obj;
}

void DataModel::Student::fromJson(const QJsonObject &json)
{
    this->student_id = json["student_id"].toInt();
    this->person_id = json["person_id"].toInt();
    this->first_name = json["first_name"].toString();
    this->second_name = json["second_name"].toString();
    this->third_name = json["third_name"].toString();
    this->fourth_name = json["fourth_name"].toString();
    // this->f

    // I need to write a function to set full name for student
}


































bool DataModel::Student::isValid() const
{
    return student_id > 0 &&
           person_id > 0 &&
           !first_name.isEmpty() &&
           !second_name.isEmpty() &&
           !third_name.isEmpty() &&
           !fourth_name.isEmpty() &&
           !student_number.isEmpty();
}

int DataModel::Student::calculateAge() const
{
    if(date_of_birth.isValid()) return 0;
    return QDate::currentDate().year() - date_of_birth.year();
}














bool StudentSummary::isValid() const
{
    return student_id > 0 && !full_name.isEmpty();
}
