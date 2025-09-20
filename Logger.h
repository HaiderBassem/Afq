// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }

    void log(const QString &message);

private:
    Logger(QObject *parent = nullptr);
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QFile m_logFile;
};

#endif // LOGGER_H
