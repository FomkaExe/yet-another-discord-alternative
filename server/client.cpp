#include "client.h"

Client::Client(qintptr socketDescriptor, QObject* parent) :
    QObject(parent),
    m_nextBlockSize(quint16(0)),
    m_client(QTcpSocket()),
    m_clientName(QString()) {
    m_client.setSocketDescriptor(socketDescriptor);
    connect(&m_client, &QIODevice::readyRead,
            this, &Client::slotClientToServer);
    connect(&m_client, &QTcpSocket::disconnected,
            this, &Client::slotClientDisconnected);
    connect(&m_server, &QTcpSocket::disconnected,
            this, &Client::slotServerDisconnected);
}

const QString& Client::getClientName() {
    return m_clientName;
}

void Client::slotClientToServer() {
    QDataStream in(&m_client);
    while (true) {
        if (!m_nextBlockSize) {
            if (m_client.bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }
        if (m_client.bytesAvailable() < m_nextBlockSize) {
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
        emit signalClientToServer(message);
        qDebug() << message;
    }
}

void Client::slotServerToClient(const QString& msg) {
    QByteArray barr;
    QDataStream out(&barr, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0) << msg;
    out.device()->seek(0);
    out << quint16(barr.size() - quint16(0));
    if (m_client.isOpen()) {
        qDebug() << m_client.write(barr);
    }
}

void Client::slotServerDisconnected() {
    emit signalRemoveDisconnectedClient(m_clientName);
    m_server.flush();
    done();
}

void Client::slotClientDisconnected() {
    emit signalRemoveDisconnectedClient(m_clientName);
    m_client.flush();
    done();
}

void Client::done() {
    m_client.close();
    m_server.close();

    deleteLater();
}
