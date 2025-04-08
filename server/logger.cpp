#include "logger.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QString>

void Logger::log(const QString& message) {
    qDebug() << "[" << QTime::currentTime().toString() << "] [" <<
        QThread::currentThreadId() << "]: " << message;
}
