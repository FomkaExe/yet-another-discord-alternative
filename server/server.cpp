#include "server.h"
#include <QDebug>
#include <QBuffer>

Server* Server::m_instance = nullptr;

Server::Server(int port) :  m_tcpServer(new QTcpServer()),
                            m_nextBlockSize(0),
                            m_clientList(QMap<QTcpSocket*, QString>()) {
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

void Server::sendToClient(const QString& message, QTcpSocket* sock) {
    QByteArray barr;
    QDataStream out(&barr, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_5);
    out << quint16(0) << message;
    out.device()->seek(0);
    out << quint16(barr.size() - quint16(0));
    sock->write(barr);
}

void Server::sendToAllClients(const QString& message) {
    for (auto it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        sendToClient(message, it.key());
    }
}

void Server::updateClientList() {
    QString clientsList("");
    for (auto it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        clientsList.append(it.value());
        clientsList.append('\t');
    }
    sendToAllClients(clientsList);
}

void Server::slotNewConnection() {
    QTcpSocket* sock = m_tcpServer->nextPendingConnection();
    if (!sock) {
        return;
    }
    m_clientList[sock];

    connect(sock, &QIODevice::readyRead,
            this, &Server::slotReadClient);
    connect(sock, &QTcpSocket::disconnected,
            this, &Server::slotClientDisconnected);
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
        if (str.last(1) == '\n') {
            str = str.first(str.size() - 1);
            m_clientList[sock] = str;
            m_nextBlockSize = 0;
            QString msg = QString("Client %1 has connected").arg(str);
            sendToAllClients(msg);
            updateClientList();
            qDebug() << msg;
            return;
        }
        QString msg = m_clientList[sock] + ": " + str;
        m_nextBlockSize = 0;
        sendToAllClients(msg);
        qDebug() << msg;
    }
}

void Server::slotClientDisconnected() {
    QTcpSocket* sock = (QTcpSocket*) sender();
    QString msg = QString("Client %1 disconnected").
                  arg(m_clientList[sock]);
    qDebug() << msg;
    m_clientList.remove(sock);
    sock->deleteLater();
    sendToAllClients(msg);
    updateClientList();
}
