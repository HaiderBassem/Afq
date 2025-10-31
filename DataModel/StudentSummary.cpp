#include "DataModel/StudentSummary.h"

namespace DataModel {

StudentSummary::StudentSummary(QObject *parent)
    : QObject(parent),
      student_id(0), rank(0), attendance_rate(0),
      completed_subjects(0), total_subjects(0),
      total_absences(0), average(0.0) {}


bool StudentSummary::isValid() const {
    return student_id > 0 && !first_name.trimmed().isEmpty();
}

// Completion rate
double StudentSummary::getCompletionRate() const {
    if (total_subjects == 0) return 0.0;
    return (static_cast<double>(completed_subjects) / total_subjects) * 100.0;
}


QString StudentSummary::getFullName() const noexcept {
    QStringList parts;
    if (!first_name.isEmpty()) parts << first_name;
    if (!second_name.isEmpty()) parts << second_name;
    if (!third_name.isEmpty()) parts << third_name;
    if (!fourth_name.isEmpty()) parts << fourth_name;
    return parts.join(" ");
}

// JSON
QJsonObject StudentSummary::toJson() const {
    QJsonObject obj;
    obj["student_id"] = student_id;
    obj["student_number"] = student_number;
    obj["first_name"] = first_name;
    obj["second_name"] = second_name;
    obj["third_name"] = third_name;
    obj["fourth_name"] = fourth_name;
    obj["class_name"] = class_name;
    obj["section"] = section;
    obj["status"] = status;
    obj["average"] = average;
    obj["rank"] = rank;
    obj["attendance_rate"] = attendance_rate;
    obj["completed_subjects"] = completed_subjects;
    obj["total_subjects"] = total_subjects;
    obj["total_absences"] = total_absences;
    obj["completion_rate"] = getCompletionRate();
    return obj;
}

void StudentSummary::fromJson(const QJsonObject &json) {
    student_id = json.value("student_id").toInt();
    student_number = json.value("student_number").toString();
    first_name = json.value("first_name").toString();
    second_name = json.value("second_name").toString();
    third_name = json.value("third_name").toString();
    fourth_name = json.value("fourth_name").toString();
    class_name = json.value("class_name").toString();
    section = json.value("section").toString();
    status = json.value("status").toString();
    average = json.value("average").toDouble();
    rank = json.value("rank").toInt();
    attendance_rate = json.value("attendance_rate").toInt();
    completed_subjects = json.value("completed_subjects").toInt();
    total_subjects = json.value("total_subjects").toInt();
    total_absences = json.value("total_absences").toInt();
}

} // namespace DataModel
