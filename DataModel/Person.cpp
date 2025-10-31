#include "DataModel/Person.h"
namespace DataModel
{

QString Person::getFullName() const
{
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}

int Person::getAge() const
{
    if (!date_of_birth.isValid()) return 0;
    return QDate::currentDate().year() - date_of_birth.year();
}

bool Person::isValid() const
{
    return !first_name.isEmpty() &&
           !second_name.isEmpty() &&
           !third_name.isEmpty() &&
           date_of_birth.isValid() &&
           person_id >= 0;
}

QJsonObject Person::toJson() const
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

void Person::fromJson(const QJsonObject& json)
{
    person_id = json["person_id"].toInt();
    first_name = json["first_name"].toString();
    second_name = json["second_name"].toString();
    third_name = json["third_name"].toString();
    fourth_name = json["fourth_name"].toString();

    QString genderStr = json["gender"].toString();
    if (!genderStr.isEmpty())
        gender = charToGender(genderStr[0].toLatin1());

    date_of_birth = QDate::fromString(json["date_of_birth"].toString(), Qt::ISODate);
    created_at = QDateTime::fromString(json["created_at"].toString(), Qt::ISODate);
    updated_at = QDateTime::fromString(json["updated_at"].toString(), Qt::ISODate);
}

QString Person::toString() const
{
    return QString("Person[ID:%1, Name:%2, Age:%3]")
        .arg(person_id)
        .arg(getFullName())
        .arg(getAge());
}
}