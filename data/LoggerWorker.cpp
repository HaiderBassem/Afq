#include "LoggerWorker.h"
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>

LoggerWorker::LoggerWorker(QObject *parent)
    : QThread(parent)
    , m_stopRequested(false)
{

    QString logDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir().mkpath(logDir); // make thhe file if not exist

    QString logPath = logDir + "/Afq.log"; // file name
    m_logFile.setFileName(logPath);

    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
    {
        qWarning() << "Failed to open log file:" << m_logFile.errorString();
    }
}

LoggerWorker::~LoggerWorker()
{
    stop();
    if (m_logFile.isOpen())
    {
        m_logFile.close();
    }
}

void LoggerWorker::enqueueMessage(const QString &message)
{
    QMutexLocker locker(&m_mutex);
    m_messageQueue.enqueue(message);
    m_condition.wakeOne();
}

void LoggerWorker::stop()
{
    QMutexLocker locker(&m_mutex);
    m_stopRequested = true;
    m_condition.wakeOne();
}

void LoggerWorker::run()
{
    QTextStream out(&m_logFile);
    out.setEncoding(QStringConverter::Utf8);

    while (true) {
        QMutexLocker locker(&m_mutex);


        while (m_messageQueue.isEmpty() && !m_stopRequested) {
            m_condition.wait(&m_mutex);
        }

        if (m_stopRequested && m_messageQueue.isEmpty()) {
            break;
        }


        QStringList messagesToProcess;
        while (!m_messageQueue.isEmpty()) {
            messagesToProcess.append(m_messageQueue.dequeue());
        }

        locker.unlock();

        for (const QString &message : messagesToProcess) {
            out << message << "\n";
        }
        out.flush();
    }
}
