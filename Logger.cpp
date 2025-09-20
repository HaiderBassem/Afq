// logger.cpp
#include "Logger.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

Logger::Logger(QObject *parent) : QObject(parent)
{
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
    + "/Afq_log.txt";

    m_logFile.setFileName(logPath);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qWarning() << "Cannot open log file:" << m_logFile.errorString();
    }
}

Logger::~Logger()
{
    if (m_logFile.isOpen())
        m_logFile.close();
}

void Logger::log(const QString &message)
{
    if (!m_logFile.isOpen())
        return;

    QTextStream out(&m_logFile);
    out.setEncoding(QStringConverter::Utf8);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
        << " | " << message << "\n";
    out.flush();
}
