#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>
#include "worker.h"

class Server : public QTcpServer {
    Q_OBJECT

private:
    Server(int port, QObject* parent = 0);
    ~Server();
    void initThreads();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    static Server* instance(int port);

private slots:
    void slotClientToServer(const QString& msg);
    void slotAddConnectedClient(const QString& client);
    void slotRemoveDisconnectedClient(const QString& client);

signals:
    void signalServerToClient(const QString& msg);

private:
    static Server* m_instance;
    quint16 m_nextBlockSize;

    size_t m_threadsCount;
    size_t m_rrCount;
    QList<QThread*> m_threadList;
    QList<Worker*> m_workerList;

    QList<QString> m_clientList;
    QMutex m_clientListMutex;
};

#endif // SERVER_H
