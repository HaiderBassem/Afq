#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <optional>
#include <QDate>
#include<QString>


#include "data/personDAL.h"
#include"data/schoolDAL.h"

using namespace DataAccess;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("Afq", "Main");

//    QDate dateOfBirth = QDate::currentDate();
    std::optional<int> i;

    // i = Person::addPerson("Hussien", "Mohammed", "Nassif", "Jassim", 'M', dateOfBirth,
    //                       "Wasit", "07709099732", "07709099732",
    //                       "HussienBassem1@gmail.com", "images/Hussien.png");
    // qDebug() << i << "\n";

    i = SchoolDAL::addSchool("Ali-Alakber");
    qDebug() << i<<"\n";

    qDebug() << SchoolDAL::isSchoolExist(7);
        qDebug() << SchoolDAL::getSchoolCount()<<"\n";

    QVector<DataModel::School> schools = SchoolDAL::getSchoolsPaginated(0, 3);
        for (int i = 0; i < schools.length(); i++) {
            qDebug() << "ID:" << schools[i].school_id << "Name:" << schools[i].name;
        }

    // i = Person::addPerson("Haider", "Bassem", "Nassif", "Jassim", 'M', dateOfBirth,
    //                       "Wasdgit", "0770909d97dg32", "0783dg363d9809",
    //                       "haideradlatdbe@gmaigdl.com", "images/Hcdaider.png");
    // qDebug() << i << "\n";

    // i = Person::deletePerson(26);
    // qDebug() << i << "\n";

    // auto people = Person::getAllPeople();

    // // طباعة الهيدر
    // qDebug().noquote() << QString("%1 | %2 | %3 | %4 | %5 | %6 | %7 | %8")
    //                           .arg("ID", 4)
    //                           .arg("First Name", 12)
    //                           .arg("Second Name", 12)
    //                           .arg("Third Name", 12)
    //                           .arg("DOB", 10)
    //                           .arg("Gender", 6)
    //                           .arg("Role", 8)
    //                           .arg("Status", 8);

    // qDebug().noquote() << "-------------------------------------------------------------------------------";

    // for (const auto& person : people) {
    //     qDebug().noquote() << QString("%1 | %2 | %3 | %4 | %5 | %6 | %7 | %8")
    //     .arg(QString::fromStdString(person.at("person_id")), 4)
    //         .arg(QString::fromStdString(person.at("first_name")), 12)
    //         .arg(QString::fromStdString(person.at("second_name")), 12)
    //         .arg(QString::fromStdString(person.at("third_name")), 12)
    //         .arg(QString::fromStdString(person.at("date_of_birth")), 10)
    //         .arg(QString::fromStdString(person.at("gender")), 6)
    //         .arg(QString::fromStdString(person.at("role")), 8)
    //         .arg(QString::fromStdString(person.at("status")), 8);
    // }

    return app.exec();
}
