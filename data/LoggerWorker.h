#pragma once
#include <QThread>
#include <QFile>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QString>

class LoggerWorker : public QThread
{
    Q_OBJECT
public:
    explicit LoggerWorker(QObject *parent = nullptr);
    ~LoggerWorker();

    void enqueueMessage(const QString &message);
    void stop();

protected:
    void run() override;

private:
    QFile m_logFile;
    QQueue<QString> m_messageQueue;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_stopRequested = false;
};
