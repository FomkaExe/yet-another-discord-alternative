#include "server.h"

Server* Server::m_instance = nullptr;

Server::Server(int port, QObject* parent/* = 0*/) :
    m_nextBlockSize(0),
    m_threadsCount(QThread::idealThreadCount()),
    m_rrCount(0),
    m_clientList(QList<QString>()) {
    listen(QHostAddress::Any, port);
    if (!isListening()) {
        qFatal() << QString("Server is not listening on port %1").arg(port);
        return;
    }
    initThreads();
    qDebug() << QString("Server is listening on port %1").arg(port);
}

Server::~Server() {
    for (size_t i = 0; i < m_threadList.size(); ++i) {
        m_threadList.at(i)->quit();
        m_threadList.at(i)->wait();
    }
    qDebug() << "Server shutting down...";
    close();
}

void Server::initThreads() {
    for (size_t i = 0; i < m_threadsCount; ++i) {
        QThread* thread = new QThread(this);
        Worker* worker = new Worker;
        worker->moveToThread(thread);
        connect(thread, &QThread::finished,
                worker, &QObject::deleteLater);

        connect(this, &Server::signalServerToClient,
                worker, &Worker::slotServerToClient);

        connect(worker, &Worker::signalClientToServer,
                this, &Server::slotClientToServer);
        connect(worker, &Worker::signalAddConnectedClient,
                this, &Server::slotAddConnectedClient);
        connect(worker, &Worker::signalRemoveDisconnectedClient,
                this, &Server::slotRemoveDisconnectedClient);

        m_threadList.push_back(thread);
        m_workerList.push_back(worker);

        thread->start();
    }
}

void Server::incomingConnection(qintptr socketDescriptor) {
    Worker* worker = m_workerList.at(m_rrCount % m_threadsCount);
    ++m_rrCount;

    QMetaObject::invokeMethod(worker, "addClient",
                              Qt::QueuedConnection,
                              Q_ARG(qintptr, socketDescriptor));
}

Server* Server::instance(int port) {
    if (!m_instance) {
        m_instance = new Server(port);
    }
    return m_instance;
}

void Server::slotClientToServer(const QString& msg) {
    emit signalServerToClient(msg);
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
    qDebug() << msg;
    emit signalServerToClient(msg);
    emit signalServerToClient(clientList);
    m_clientListMutex.unlock();
}

void Server::slotRemoveDisconnectedClient(const QString& client) {
    m_clientListMutex.lock();
    if (m_clientList.contains(client)) {
        QString clientList;
        qsizetype size = m_clientList.size();
        for (qsizetype i = 0; i < size; ++i) {
            if (m_clientList.at(i) == client) {
                m_clientList.removeAt(i);
                --i;
                --size;
            } else {
                clientList.push_back(m_clientList.at(i));
                clientList.push_back('\t');
            }
        }
        QString msg = QString("Client %1 has disconnected").arg(client);
        qDebug() << msg;
        emit signalServerToClient(msg);
        emit signalServerToClient(clientList);
    }
    m_clientListMutex.unlock();
}
