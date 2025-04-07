#include "logger.h"
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QString>

void Logger::log(const QString& message) {
    qDebug() << "[" << QTime::currentTime().toString() << "] [" <<
        QThread::currentThreadId() << "]: " << message;
    // qDebug() << QString("[%1][%2]: %3").arg(QTime::currentTime().toString())
                    // .arg(qobject_cast<QObject*>(QThread::currentThread()))
                    // .arg(message);
}
