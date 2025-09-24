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
    year.name = "2026/2025";
    year.start_date = QDate::currentDate();
    year.end_date = QDate(2026, 6, 1);
    year.is_active = true;
    year.holidays_count = 30;

    DataAccess::AcademicYearDataHandler::addAcademicYear(year);

    return app.exec();
}

