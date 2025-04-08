#include "worker.h"

void Worker::addClient(qintptr socketDescriptor) {
    Client* client = new Client(socketDescriptor);
    client->setParent(this);
    m_clientList.push_back(client);

    connect(client, &Client::signalClientToServer,
            this, &Worker::signalClientToServer);
    connect(client, &Client::signalAddConnectedClient,
            this, &Worker::signalAddConnectedClient);
    connect(client, &Client::signalRemoveDisconnectedClient,
            this, &Worker::signalRemoveDisconnectedClient);

    connect(client, &Client::signalRemoveDisconnectedClient,
            this, &Worker::slotClientDisconnected);
}

void Worker::slotServerToClient(const QString& msg) {
    for (int i = 0; i < m_clientList.size(); ++i) {
        if (m_clientList.at(i)) {
            m_clientList.at(i)->slotServerToClient(msg);
        }
    }
}

void Worker::slotClientDisconnected() {
    for (qsizetype i = 0; i < m_clientList.size(); ++i) {
        Client* client = m_clientList.at(i);
        if (client == sender()) {
            disconnect(client, &Client::signalRemoveDisconnectedClient,
                    this, &Worker::slotClientDisconnected);
            m_clientList.removeAt(i);
        }
    }
}
