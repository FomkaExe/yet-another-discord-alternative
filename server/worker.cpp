#include "worker.h"
#include "logger.h"

#define log Logger::log


void Worker::addClient(qintptr socketDescriptor) {
    log("Worker::addClient");
    Client* client = new Client(socketDescriptor);
    client->setParent(this);
    m_clientList.push_back(client);
    connect(client, &Client::signalClientToServer,
            this, [=](const QString& msg) {
                emit signalClientToServer(msg);
            });
    connect(client, &Client::signalAddConnectedClient,
            this, [=](const QString& client) {
                emit signalAddConnectedClient(client);
            });
    connect(client, &Client::signalRemoveDisconnectedClient,
            this, [=](const QString& client) {
                emit signalRemoveDisconnectedClient(client);
            });
    connect(client, &Client::signalRemoveDisconnectedClient,
            this, &Worker::slotClientDisconnected);
    log("Worker::addClient END");
}

void Worker::slotServerToClient(const QString& msg) {
    log("Worker::slotServerToClient");
    for (int i = 0; i < m_clientList.size(); ++i) {
        m_clientList.at(i)->slotServerToClient(msg);
    }
    log("Worker::slotServerToClient END");
}

void Worker::slotClientDisconnected() {
    log("Worker::slotClientDisconnected");
    for (qsizetype i = 0; i < m_clientList.size(); ++i) {
        Client* client = m_clientList.at(i);
        if (client == sender()) {
            disconnect(client, &Client::signalRemoveDisconnectedClient,
                    this, &Worker::slotClientDisconnected);
            m_clientList.removeAt(i);
        }
    }
    log("Worker::slotClientDisconnected END");
}
