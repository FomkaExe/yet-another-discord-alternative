#include "clientthread.h"

ClientThread::ClientThread(qintptr socketDescriptor) :
    m_socketDescriptor(socketDescriptor),
    m_thread(new QThread()),
    m_nextBlockSize(quint16(0)),
    m_tcpSocket(nullptr),
    m_clientName(QString()) {
}

void ClientThread::start() {
    m_tcpSocket = new QTcpSocket();
    if (!m_tcpSocket->setSocketDescriptor(m_socketDescriptor)) {
        emit signalError(m_tcpSocket->error());
        return;
    }

    connect(m_tcpSocket, &QIODevice::readyRead,
            this, &ClientThread::slotReadClient);

    connect(m_tcpSocket, &QAbstractSocket::disconnected,
            this, &ClientThread::slotClientDisconnected);
}

void ClientThread::slotReadClient() {
    qDebug() << "SlotRead thread: " << QThread::currentThreadId();
    QDataStream in(m_tcpSocket);
    while (true) {
        if (!m_nextBlockSize) {
            if (m_tcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }
        if (m_tcpSocket->bytesAvailable() < m_nextBlockSize) {
            break;
        }
        QString str;
        in >> str;
        if (str.last(1) == '\n') {
            str = str.first(str.size() - 1);
            m_nextBlockSize = 0;
            m_clientName = str;
            emit signalAddConnectedClient(m_clientName);
            return;
        }
        QString message = m_clientName + ": " + str;
        m_nextBlockSize = 0;
        emit signalNewMessageServer(message);
    }
}

void ClientThread::slotWriteClient(const QString& msg) {
    QByteArray barr;
    QDataStream out(&barr, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0) << msg;
    out.device()->seek(0);
    out << quint16(barr.size() - quint16(0));
    m_tcpSocket->write(barr);
}

void ClientThread::slotClientDisconnected() {
    emit signalRemoveDisconnectedClient(m_clientName);
    emit signalFinished();
}
