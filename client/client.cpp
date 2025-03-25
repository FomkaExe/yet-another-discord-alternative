#include "client.h"

Client::Client(QObject *parent)
    : QObject{parent},
    m_socket(new QTcpSocket()),
    m_nextBlockSize(0) {
    connect(m_socket, &QAbstractSocket::errorOccurred,
            this, &Client::slotErrorOccurred);
    connect(m_socket, &QIODevice::readyRead,
            this, &Client::slotReadyRead);
    connect(m_socket, &QAbstractSocket::connected,
            this, &Client::slotEmitConnected);
}

void Client::connectToServer(const QString& ipv4,
                             int port,
                             const QString& nickname) {
    if (m_socket->state() != 0) {
        emit signalSocketError(QAbstractSocket::NetworkError);
        return;
    }
    m_socket->connectToHost(ipv4, port);
    sendToServer(nickname + "\n");
}

void Client::disconnectFromServer() {
    m_socket->disconnectFromHost();
}

qint64 Client::sendToServer(const QString& message) {
    QByteArray barr;
    QDataStream out(&barr, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0) << message;
    out.device()->seek(0);
    out << quint16(barr.size() - sizeof(quint16));
    return m_socket->write(barr);
}

void Client::slotErrorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << socketError;
    emit signalSocketError(socketError);
}

void Client::slotReadyRead() {
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_6_5);
    while (true) {
        if (!m_nextBlockSize) {
            if (in.device()->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }

        if (in.device()->bytesAvailable() < m_nextBlockSize - sizeof(quint16)) {
            break;
        }
        QString str;
        in >> str;
        emit readyReadSuccess(str);

        m_nextBlockSize = 0;
    }
}

void Client::slotEmitConnected() {
    emit connectedToServer();
}
