#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <optional>
#include <QDate>
#include<QString>

#include"data/PeopleDataHandler.h"
#include"data/GuardianDataHandler.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("Afq", "Main");
    DataModel::Guardian g;
    g.person_id = 1;
    g.occupation = "Dr.";
    g.work_phone = "07709099732";
    g.education_level = "PhD";


    //DataAccess::GuardianDataHandler::isGuardianExists(2) ? qDebug() << "Hi" : qDebug() <<"bye";
    // qDebug() <<DataAccess::AcademicYearDataHandler::getDaysElapsedInAcademicYear(5) << "\n";

    return app.exec();
}

