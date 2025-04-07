#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "client.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void addClient(qintptr socketDescriptor);

public slots:
    void slotServerToClient(const QString& msg);
    void slotClientDisconnected();

signals:
    void signalClientToServer(const QString& msg);
    void signalAddConnectedClient(const QString& client);
    void signalRemoveDisconnectedClient(const QString& client);
    void signalServerToClient(const QString& msg);

private:
    QList<Client*> m_clientList;
};

#endif // WORKER_H
