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
        emit signalSocketError("ERROR: You are already connected");
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

void Client::slotErrorOccurred(const QAbstractSocket::SocketError& socketError){
    switch(socketError) {
    case QAbstractSocket::UnknownSocketError:
        emit signalSocketError("ERROR: Unidentified error");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit signalSocketError("ERROR: Connection refused");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        emit signalSocketError("ERROR: Remote host closed the connection");
        break;
    case QAbstractSocket::HostNotFoundError:
        emit signalSocketError("ERROR: Host address not found");
        break;
    case QAbstractSocket::SocketAccessError:
        emit signalSocketError("ERROR: Socket access error");
        break;
    case QAbstractSocket::SocketResourceError:
        emit signalSocketError("ERROR: Local system ran out of resources");
        break;
    case QAbstractSocket::SocketTimeoutError:
        emit signalSocketError("ERROR: Socket operation timed out. Try again");
        break;
    case QAbstractSocket::DatagramTooLargeError:
        emit signalSocketError("ERROR: Datagram too large");
        break;
    case QAbstractSocket::NetworkError:
        emit signalSocketError("ERROR: Network error. Try again");
        break;
    case QAbstractSocket::AddressInUseError:
        emit signalSocketError("ERROR: Address is already in use");
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        emit signalSocketError("ERROR: Address not available");
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        emit signalSocketError("ERROR: Socket operation not supported");
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        emit signalSocketError("ERROR: Last operation attempted has not finished yet");
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        emit signalSocketError("ERROR: Proxy authentication required");
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        emit signalSocketError("ERROR: SSL handshake failed");
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        emit signalSocketError("ERROR: Proxy connection refused");
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        emit signalSocketError("ERROR: Proxy connection closed unexpectedly");
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        emit signalSocketError("ERROR: Proxy connection timeout");
        break;
    case QAbstractSocket::ProxyNotFoundError:
        emit signalSocketError("ERROR: Proxy address not found");
        break;
    case QAbstractSocket::ProxyProtocolError:
        emit signalSocketError("ERROR: Proxy protocol error");
        break;
    case QAbstractSocket::OperationError:
        emit signalSocketError("ERROR: Operation not permitted by socket");
        break;
    case QAbstractSocket::SslInternalError:
        emit signalSocketError("ERROR: SSL internal error");
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        emit signalSocketError("ERROR: SLL invalid user data");
        break;
    case QAbstractSocket::TemporaryError:
        emit signalSocketError("ERROR: Temporary error. Try again");
        break;
    default:
        break;
    }
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
        if (str.last(1) == '\t') {
            QStringList clientsList = str.split('\t');
            m_nextBlockSize = 0;
            emit clientListUpdated(clientsList);
            return;
        }
        m_nextBlockSize = 0;
        emit readyReadSuccess(str);
    }
}

void Client::slotEmitConnected() {
    emit connectedToServer();
}
