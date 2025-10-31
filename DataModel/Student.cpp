#include "DataModel/Student.h"
#include"DataModel/Enrollment.h"
#include"DataModel/StudentEnrollment.h"

#include <QRegularExpression>
#include <QStringList>

namespace DataModel {

// Constructor
Student::Student(QObject *parent)
    : QObject(parent),
      student_id(0), person_id(0), student_number(""),
      first_name(""), second_name(""), third_name(""), fourth_name(""),
      gender(Gender::Male), status(StudentStatus::Inactive),
      type(StudentType::Regular), age(0),
      current_class_id(0), current_year_id(0),
      current_average(0.0), current_rank(0), total_absences(0) {}

// Copy constructor
Student::Student(const Student& other) {
    *this = other;
}

// Copy assignment
Student& Student::operator=(const Student& other) {
    if (this != &other) {
        student_id = other.student_id;
        person_id = other.person_id;
        first_name = other.first_name;
        second_name = other.second_name;
        third_name = other.third_name;
        fourth_name = other.fourth_name;
        gender = other.gender;
        date_of_birth = other.date_of_birth;
        status = other.status;
        type = other.type;
        enrollment_date = other.enrollment_date;
        graduation_date = other.graduation_date;
        current_class_id = other.current_class_id;
        current_class_name = other.current_class_name;
        section = other.section;
        current_year_id = other.current_year_id;
        current_year_name = other.current_year_name;
        current_average = other.current_average;
        current_rank = other.current_rank;
        total_absences = other.total_absences;
    }
    return *this;
}

// Move constructor
Student::Student(Student&& other) noexcept {
    *this = std::move(other);
}

// Move assignment
Student& Student::operator=(Student&& other) noexcept {
    if (this != &other) {
        student_id = other.student_id;
        person_id = other.person_id;
        first_name = std::move(other.first_name);
        second_name = std::move(other.second_name);
        third_name = std::move(other.third_name);
        fourth_name = std::move(other.fourth_name);
        gender = other.gender;
        date_of_birth = other.date_of_birth;
        status = other.status;
        type = other.type;
        enrollment_date = other.enrollment_date;
        graduation_date = other.graduation_date;
        current_class_id = other.current_class_id;
        current_class_name = std::move(other.current_class_name);
        section = std::move(other.section);
        current_year_id = other.current_year_id;
        current_year_name = std::move(other.current_year_name);
        current_average = other.current_average;
        current_rank = other.current_rank;
        total_absences = other.total_absences;
    }
    return *this;
}

// Destructor
Student::~Student() = default;


QString Student::getFullName() const noexcept {
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}


bool Student::isValid() const {
    return student_id > 0 &&
           person_id > 0 &&
           !first_name.isEmpty() &&
           !second_name.isEmpty() &&
           date_of_birth.isValid() &&
           !student_number.isEmpty();
}

// Type and status checks
bool Student::isActive() const { return status == StudentStatus::Active; }
bool Student::isHonorsStudent() const { return type == StudentType::Honors; }
bool Student::isRegularStudent() const { return type == StudentType::Regular; }
bool Student::isWeekStudent() const { return type == StudentType::Weak; }


int Student::calculateAge() const {
    if (!date_of_birth.isValid()) return 0;
    return QDate::currentDate().year() - date_of_birth.year();
}


QString Student::toString() const {
    return QString("Student[ID:%1, Number:%2, Name:%3, Status:%4, Class:%5]")
            .arg(student_id)
            .arg(student_number)
            .arg(getFullName())
            .arg(studentStatusToString(status))
            .arg(current_class_name);
}

// JSON serialization
QJsonObject Student::toJson() const {
    QJsonObject obj;
    obj["student_id"] = student_id;
    obj["person_id"] = person_id;
    obj["first_name"] = first_name;
    obj["second_name"] = second_name;
    obj["third_name"] = third_name;
    obj["fourth_name"] = fourth_name;
    obj["full_name"] = getFullName();
    obj["gender"] = static_cast<int>(gender);
    obj["date_of_birth"] = date_of_birth.toString(Qt::ISODate);
    obj["age"] = calculateAge();
    obj["student_number"] = student_number;
    obj["status"] = static_cast<int>(status);
    obj["type"] = static_cast<int>(type);
    obj["current_class_id"] = current_class_id;
    obj["current_class_name"] = current_class_name;
    obj["section"] = section;
    obj["current_year_id"] = current_year_id;
    obj["current_year_name"] = current_year_name;
    obj["current_average"] = current_average;
    obj["current_rank"] = current_rank;
    obj["total_absences"] = total_absences;
    return obj;
}

void Student::fromJson(const QJsonObject &json) {
    student_id = json.value("student_id").toInt();
    person_id = json.value("person_id").toInt();
    first_name = json.value("first_name").toString();
    second_name = json.value("second_name").toString();
    third_name = json.value("third_name").toString();
    fourth_name = json.value("fourth_name").toString();
    student_number = json.value("student_number").toString();
    gender = static_cast<Gender>(json.value("gender").toInt());
    date_of_birth = QDate::fromString(json.value("date_of_birth").toString(), Qt::ISODate);
    status = static_cast<StudentStatus>(json.value("status").toInt());
    type = static_cast<StudentType>(json.value("type").toInt());
    current_class_id = json.value("current_class_id").toInt();
    current_class_name = json.value("current_class_name").toString();
    section = json.value("section").toString();
    current_year_id = json.value("current_year_id").toInt();
    current_year_name = json.value("current_year_name").toString();
    current_average = json.value("current_average").toDouble();
    current_rank = json.value("current_rank").toInt();
    total_absences = json.value("total_absences").toInt();
}

} // namespace DataModel
