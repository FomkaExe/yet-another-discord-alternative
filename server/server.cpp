#include "server.h"
#include <QDebug>
#include <QBuffer>

Server* Server::m_instance = nullptr;

Server::Server(int port) :  m_tcpServer(new QTcpServer()),
                            m_nextBlockSize(0),
                            m_clientList(QList<QTcpSocket*>()) {
    if (!m_tcpServer->listen(QHostAddress::Any, port)) {
        qFatal() << QString("Server is not listening on port %1").arg(port);
        return;
    }
    qDebug() << QString("Server is listening on port %1").arg(port);

    connect(m_tcpServer, &QTcpServer::newConnection,
            this, &Server::slotNewConnection);
}

Server::~Server() {
    qDebug() << "Server shutting down...";
    m_tcpServer->close();
    delete m_tcpServer;
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
    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0) << message;
    out.device()->seek(0);
    out << quint16(barr.size() - quint16(0));
    qDebug() << sock->write(barr);
}

void Server::sendToAllClients(QString& message) {
    for (int i = 0; i < m_clientList.size(); ++i) {
        sendToClient(message, m_clientList.at(i));
    }
}

void Server::slotNewConnection() {
    QTcpSocket* sock = m_tcpServer->nextPendingConnection();
    if (!sock) {
        return;
    }
    m_clientList.push_back(sock);

    connect(sock, &QIODevice::readyRead,
            this, &Server::slotReadClient);
    connect(sock, &QTcpSocket::disconnected,
            this, &Server::slotClientDisconnected);

    QString msg = QString("Client %1:%2 has connected").
                  arg(sock->peerAddress().toString()).arg(sock->peerPort());
    qDebug() << msg;
    sendToAllClients(msg);
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
        QString message = sock->peerAddress().toString() + sock->peerPort() +
                          ": " + str;
        m_nextBlockSize = 0;
        sendToAllClients(message);
    }
}

void Server::slotClientDisconnected() {
    QTcpSocket* sock = (QTcpSocket*) sender();
    QString msg = QString("Client %1:%2 disconnected").
                  arg(sock->peerAddress().toString()).arg(sock->peerPort());
    sock->deleteLater();
    for (int i = 0; i < m_clientList.size(); ++i) {
        if (m_clientList.at(i) == sock) {
            m_clientList.removeAt(i);
        }
    }
    sendToAllClients(msg);
}
