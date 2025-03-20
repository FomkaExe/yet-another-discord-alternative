#include "server.h"
#include <QDebug>
#include <QBuffer>

Server* Server::m_instance = nullptr;

Server::Server(int port) :  m_tcpServer(new QTcpServer()),
                            m_nextBlockSize(0),
                            m_clientList(QList<QTcpSocket*>()) {
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qFatal() << QString("Server is not listening on port %1").arg(port);
        m_tcpServer->close();
        return;
    }
    qDebug() << QString("Server is listening on port %1").arg(port);
    connect(m_tcpServer, &QTcpServer::newConnection,
            this, &Server::slotNewConnection);
}

Server::~Server() {
    m_tcpServer->close();
}

Server* Server::instance(int port) {
    if (!m_instance) {
        m_instance = new Server(port);
    }
    return m_instance;
}

void Server::sendToClient(QString& message, QTcpSocket* sock) {
    QByteArray barr;
    QDataStream out(&barr, QIODevice::WriteOnly);
    out << quint16(0) << message;
    out.device()->seek(0);
    out << (barr.size() - quint16(0));
    sock->write(barr);
    qDebug() << sock->peerAddress().toString() + ": " + message;
}

void Server::sendToAllClients(QString& message) {
    for (auto client : m_clientList) {
        sendToClient(message, client);
    }
}

void Server::slotNewConnection() {
    QTcpSocket* sock = (QTcpSocket*) sender();
    if (!sock) {
        return;
    }
    m_clientList.push_back(sock);

    connect(sock, &QIODevice::readyRead,
            this, &Server::slotReadClient);
    connect(sock, &QTcpSocket::disconnected,
            this, &Server::slotClientDisconnected);

    QString msg = QString("Client %1 has connected").
                  arg(sock->peerAddress().toString());
    sendToClient(msg, sock);
}

void Server::slotReadClient() {
    QTcpSocket* sock = (QTcpSocket*) sender();
    QDataStream in(sock);
    while (true) {
        if (!m_nextBlockSize) {
            if (sock->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nextBlockSize;
        }
        if (sock->bytesAvailable() < m_nextBlockSize) {
            break;
        }
        QString str;
        in >> str;
        QString message = sock->peerAddress().toString() + ": " + str;
        m_nextBlockSize = 0;
        sendToAllClients(message);
    }
}

void Server::slotClientDisconnected() {
    QTcpSocket* sock = (QTcpSocket*) sender();
    QString peerAddress = sock->peerAddress().toString();
    QString msg = QString("Client %1 has disconnected").arg(peerAddress);
    sendToClient(msg, sock);
}
