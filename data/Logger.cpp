#include "Logger.h"
#include <QDateTime>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
{

    m_worker.start();
}

Logger::~Logger()
{

}

void Logger::log(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString formattedMessage = QString("%1 | %2").arg(timestamp, message);
    m_worker.enqueueMessage(formattedMessage);
}

void Logger::debug(const QString &message)
{
    log("[DEBUG] " + message);
}

void Logger::info(const QString &message)
{
    log("[INFO] " + message);
}

void Logger::warning(const QString &message)
{
    log("[WARNING] " + message);
}

void Logger::error(const QString &message)
{
    log("[ERROR] " + message);
}
