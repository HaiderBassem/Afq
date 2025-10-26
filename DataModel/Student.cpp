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





DataModel::Student::Student(const Student& other) {
    set_student_id(other.get_student_id());
    set_person_id(other.get_person_id());
    set_first_name(other.get_first_name());
    set_second_name(other.get_second_name());
    set_third_name(other.get_third_name());
    set_fourth_name(other.get_fourth_name());
    set_gender(genderToQChar(other.gender));
    set_date_of_birth(other.get_date_of_birth());
    set_status(other.get_status());
    set_enrollment_date(other.get_enrollment_date());
    set_graduation_date(other.get_graduation_date());
    set_section(other.get_section());
}

// Implement copy assignment operator
DataModel::Student& DataModel::Student::operator=(const Student& other) {
    if (this != &other) {
        set_student_id(other.get_student_id());
        set_person_id(other.get_person_id());
        set_first_name(other.get_first_name());
        set_second_name(other.get_second_name());
        set_third_name(other.get_third_name());
        set_fourth_name(other.get_fourth_name());
        set_gender(genderToQChar(other.gender));
        set_date_of_birth(other.get_date_of_birth());
        set_status( other.get_status());
        set_enrollment_date(other.get_enrollment_date());
        set_graduation_date(other.get_graduation_date());
        set_section(other.get_section());
    }
    return *this;
}

DataModel::Student::Student(Student &&other) noexcept
{
    set_student_id(other.get_student_id());
    set_person_id(other.get_person_id());
    set_first_name(std::move(other.first_name));
    set_second_name(std::move(other.second_name));
    set_third_name(std::move(other.third_name));
    set_fourth_name(std::move(other.fourth_name));
    set_gender(std::move(genderToQChar(other.gender)));
    set_date_of_birth(other.get_date_of_birth());
    set_status(other.get_status());
    set_enrollment_date(other.get_enrollment_date());
    set_graduation_date(other.get_graduation_date());
    set_section(std::move(other.section));  

}
DataModel::Student &DataModel::Student::operator=(Student &&other) noexcept
{
    // TODO: insert return statement here
    if (this != &other) {
        set_student_id(other.get_student_id());
        set_person_id(other.get_person_id());
        set_first_name(std::move(other.first_name));
        set_second_name(std::move(other.second_name));
        set_third_name(std::move(other.third_name));
        set_fourth_name(std::move(other.fourth_name));
        set_gender(std::move(genderToQChar(other.gender)));
        set_date_of_birth(other.get_date_of_birth());
        set_status(other.get_status());
        set_enrollment_date(other.get_enrollment_date());
        set_graduation_date(other.get_graduation_date());
        set_section(std::move(other.section));
        return *this;
}
}


DataModel::Student::~Student()
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



bool DataModel::Student::set_is_graduated(bool graduated)
{
    this->is_graduated = graduated;
    return false;
}

bool DataModel::Student::set_is_repeated(bool repeated)
{
    this->is_repeated = repeated;
    return false;
}

bool DataModel::Student::set_is_ministerial_exam(bool ministerial)
{
    this->is_ministerial_exam = ministerial;
    return false;
}

bool DataModel::Student::set_is_eligible_for_exam(bool eligible)
{
    this->is_eligible_for_exam = eligible;
    return false;
}

int DataModel::Student::calculateAge() const
{
    if(!this->date_of_birth.isValid()) return 0;
    return QDate::currentDate().year() - this->date_of_birth.year();
}

int DataModel::Student::get_total_absences() const
{
    return this->total_absences;
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







// getter

int DataModel::Student::get_student_id() const
{
    return this->student_id;
}
int DataModel::Student::get_person_id() const
{
    return this->person_id;
}
QString DataModel::Student::get_first_name() const
{
    return this->first_name;
}
QString DataModel::Student::get_second_name() const
{
    return this->second_name;
}
QString DataModel::Student::get_third_name() const
{
    return this->third_name;
}
QString DataModel::Student::get_fourth_name() const
{
    return this->fourth_name;
}
QString DataModel::Student::get_student_number() const
{
    return this->student_number;
}
DataModel::Gender DataModel::Student::get_gender() 
{
    return this->gender;
}
QDate DataModel::Student::get_date_of_birth() const
{
    return this->date_of_birth;
}
int DataModel::Student::get_age() const
{
    return this->calculateAge();
}
DataModel::StudentStatus DataModel::Student::get_status() const
{
    return this->status;
}
DataModel::StudentType DataModel::Student::get_type() const
{
    return this->type;
}
QDate DataModel::Student::get_enrollment_date() const
{
    return this->enrollment_date;
}
QDate DataModel::Student::get_graduation_date() const
{
    return this->graduation_date;
}
QString DataModel::Student::get_phone() const
{
    return this->phone;
}
int DataModel::Student::get_current_class_id() const
{
    return this->current_class_id;
}
QString DataModel::Student::get_current_class_name() const
{
    return this->current_class_name;
}
QString DataModel::Student::get_section() const
{
    return this->section;
}
int DataModel::Student::get_current_year_id() const
{
    return this->current_year_id;
}
QString DataModel::Student::get_current_year_name() const
{
    return this->current_year_name;
}
double DataModel::Student::get_current_average() const
{
    return this->current_average;
}
int DataModel::Student::get_current_rank() const
{
    return this->current_rank;
}
int DataModel::Student::get_total_absences() const
{
    return this->total_absences;
}

// setter

bool DataModel::Student::set_student_id(int student_id)
{
    if(student_id > 0)
    {
        this->student_id = student_id;
        return true;
    }
    return false;
}

bool DataModel::Student::set_person_id(int person_id)
{
    if(person_id > 0)
    {
        this->person_id = person_id;
        return true;
    }
    return false;
}

bool DataModel::Student::set_first_name(const QString &fname)
{
    if(!fname.isEmpty())
    {
        this->first_name = fname;
        return true;
    }
    return false;
}

bool DataModel::Student::set_second_name(const QString &sname)
{
    if(!sname.isEmpty())
    {
        this->second_name = sname;
        return true;
    }
    return false;
}

bool DataModel::Student::set_third_name(const QString &tname)
{
    if(!tname.isEmpty())
    {
        this->third_name = tname;
        return true;
    }
    return false;
}

bool DataModel::Student::set_fourth_name(const QString &ftname)
{
    if(!ftname.isEmpty())
    {
        this->fourth_name = ftname;
        return true;
    }
    return false;
}

bool DataModel::Student::set_gender(QChar gender)
{
    if(gender.isLetter() && gender == 'M' || 'm' || 'F' || 'f')
    {
        this->gender = DataModel::charToGender(gender);
        return true;
    }
    return false;
}

bool DataModel::Student::set_date_of_birth(const QDate &dob)
{
    auto check = [](const QDate& d) {return (d.year() - QDate::currentDate().year()) > 6 ;};
    if(dob.isValid() && check(dob))
    {
        this->date_of_birth = dob;
        return true;
    }
    return false;
}

bool DataModel::Student::set_status(const StudentStatus &status)
{
    if(status != StudentStatus::Unknown)
    {
        this->status = status;
        return true;
    }
    return false;
}

bool DataModel::Student::set_enrollment_date(const QDate &ed)
{
    if(ed.isValid())
    {
        this->enrollment_date = ed;
        return true;
    }
    return false;
}

bool DataModel::Student::set_graduation_date(const QDate &gd)
{
    if(gd.isValid())
    {
        this->enrollment_date = gd;
        return true;
    }
    return false;
}
bool DataModel::Student::set_phone(const QString &phone)
{
    static const QRegularExpression regex(R"(^\+?\d+$)");

    if (!regex.match(phone).hasMatch())
        return false;

    this->phone = phone;
    return true;
}

bool DataModel::Student::set_section(const QString &section)
{
    if (!QRegularExpression(R"(^[A-Z]$)").match(section).hasMatch())
        return false;

    this->section = section;
    return true;
}

bool DataModel::Student::set_age(int age)
{
    if(age >= 6 && age <= 100)
    {
        this->age = age;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_class_id(int class_id)
{
    if(class_id > 0)
    {
        this->current_class_id = class_id;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_class_name(const QString &class_name)
{
    if(!class_name.isEmpty())
    {
        this->current_class_name = class_name;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_year_id(int year_id)
{
    if(year_id > 0)
    {
        this->current_year_id = year_id;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_year_name(const QString &year_name)
{
    if(!year_name.isEmpty())
    {
        this->current_year_name = year_name;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_average(double average)
{
    if(average >= 0.0 && average <= 100.0)
    {
        this->current_average = average;
        return true;
    }
    return false;
}

bool DataModel::Student::set_current_rank(int rank)
{
    if(rank > 0)
    {
        this->current_rank = rank;
        return true;
    }
    return false;
}

bool DataModel::Student::set_repeat_count(int)
{
    if(repeat_count >= 0)
    {
        this->repeat_count = repeat_count;
        return true;
    }
    return false;
}

bool DataModel::Student::set_student_number(const QString &student_number)
{
    if(!student_number.isEmpty())
    {
        this->student_number = student_number;
        return true;
    }
    return false;
}

bool DataModel::Student::set_grade_level(int level)
{
    if(level > 0)
    {
        this->grade_level = level;
        return true;
    }
    return false;
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






































