#pragma once
#include "LoggerWorker.h"
#include <QString>

class Logger
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // الحصول على النسخة الوحيدة
    static Logger& instance();

    // وظائف التسجيل
    void log(const QString &message);
    void debug(const QString &message);
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);

private:
    Logger();
    ~Logger();

    LoggerWorker m_worker;
};
