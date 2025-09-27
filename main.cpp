#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <optional>
#include <QDate>
#include<QString>

#include"data/PeopleDataHandler.h"
#include"data/AcademicYearDataHandler.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("Afq", "Main");
    DataModel::AcademicYear year;
    year.name = "2027/2026";
    year.start_date = QDate(2026, 10, 1);
    year.end_date = QDate(2027, 6, 1);
    year.is_active = true;
    year.holidays_count = 30;

    DataAccess::AcademicYearDataHandler::getAcademicYearDuration(5);
    // qDebug() <<DataAccess::AcademicYearDataHandler::getDaysElapsedInAcademicYear(5) << "\n";

    return app.exec();
}

