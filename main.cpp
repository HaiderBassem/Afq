#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <optional>
#include <QDate>
#include<QString>

#include"data/PeopleDataHandler.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);

    engine.loadFromModule("Afq", "Main");

    return app.exec();
}

