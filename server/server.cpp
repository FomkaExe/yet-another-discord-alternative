#include "server.h"
#include <QDebug>

Server* Server::m_instance = nullptr;

Server::Server(int port, QObject* parent/* = 0*/) :
    m_nextBlockSize(0),
    m_clientList(QList<QString>()) {
    listen(QHostAddress::Any, port);
    if (!isListening()) {
        qFatal() << QString("Server is not listening on port %1").arg(port);
        return;
    }
    qDebug() << QString("Server is listening on port %1").arg(port);
}

Server::~Server() {
    qDebug() << "Server shutting down...";
    close();
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QThread* thread = new QThread;
    ClientThread* clientThread = new ClientThread(socketDescriptor);

    clientThread->moveToThread(thread);

    // thread lifetime control
    connect(thread, &QThread::started,
            clientThread, &ClientThread::start);

    connect(clientThread, &ClientThread::signalFinished,
            thread, &QThread::quit);

    connect(clientThread, &ClientThread::signalFinished,
            clientThread, &ClientThread::deleteLater);

    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);

    // clientThread logic
    connect(clientThread, &ClientThread::signalError,
            this, &Server::slotClientThreadError);

    connect(clientThread, &ClientThread::signalNewMessageServer,
            this, &Server::slotNewMessage);

    connect(clientThread, &ClientThread::signalAddConnectedClient,
            this, &Server::slotAddConnectedClient);

    connect(clientThread, &ClientThread::signalRemoveDisconnectedClient,
            this, &Server::slotRemoveDisconnectedClient);

    connect(this, &Server::signalNewMessageClientThread,
            clientThread, &ClientThread::slotWriteClient);

    thread->start();
}

Server* Server::instance(int port) {
    if (!m_instance) {
        m_instance = new Server(port);
    }
    return m_instance;
}

void Server::slotClientThreadError(QTcpSocket::SocketError socketError) {
    qDebug() << socketError;
}

void Server::slotNewMessage(const QString& msg) {
    emit signalNewMessageClientThread(msg);
    qDebug() << msg;
}

void Server::slotAddConnectedClient(const QString& client) {
    m_clientListMutex.lock();
    if (!m_clientList.contains(client)) {
        m_clientList.push_back(client);
    }
    QString clientList;
    for (auto it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        clientList.push_back(*it);
        clientList.push_back('\t');
    }
    QString msg = QString("Client %1 has connected").arg(client);
    emit signalNewMessageClientThread(msg);
    emit signalNewMessageClientThread(clientList);
    m_clientListMutex.unlock();
}

void Server::slotRemoveDisconnectedClient(const QString& client) {
    m_clientListMutex.lock();
    if (m_clientList.contains(client)) {
        QString clientList;
        int count = 0;
        for (int i = 0; i < m_clientList.size(); ++i) {
            if (m_clientList.at(i) == client) {
                m_clientList.remove(i);
                continue;
            }
            clientList.push_back(m_clientList.at(i));
            clientList.push_back('\t');
        }
        QString msg = QString("Client %1 has disconnected").arg(client);
        qDebug() << msg;
        emit signalNewMessageClientThread(msg);
        emit signalNewMessageClientThread(clientList);
    }
    m_clientListMutex.unlock();
}
