#include "server.h"
#include <QDebug>

Server* Server::m_instance = nullptr;

Server::Server(int port, QObject* parent/* = 0*/) :
    m_nextBlockSize(0),
    m_clientList(QMap<ClientThread*, QString>()) {
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
    m_clientList[clientThread];

    clientThread->moveToThread(thread);

    connect(thread, &QThread::started,
            clientThread, &ClientThread::start);

    connect(clientThread, &ClientThread::signalFinished,
            thread, &QThread::quit);

    connect(clientThread, &ClientThread::signalFinished,
            clientThread, &ClientThread::deleteLater);

    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);

    connect(clientThread, &ClientThread::signalError,
            this, &Server::slotClientThreadError);

    connect(clientThread, &ClientThread::signalNewMessageServer,
            this, &Server::slotNewMessage);

    connect(clientThread, &ClientThread::signalClientListUpdated,
            this, &Server::slotClientListUpdated);

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

void Server::slotNewMessage(QString msg) {
    emit signalNewMessageClientThread(msg);
    qDebug() << msg;
}

void Server::slotClientListUpdated(QString client) {
    m_clientList[static_cast<ClientThread*>(sender())] = client;
    QString clientList;
    for (auto it = m_clientList.begin(); it != m_clientList.end(); ++it) {
        clientList += it.value();
        clientList += '\t';
    }
    slotNewMessage(clientList);
}
