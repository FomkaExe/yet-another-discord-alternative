#ifndef SERVER_H
#define SERVER_H
#include "clientthread.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QMutex>

class Server : public QTcpServer {
    Q_OBJECT

private:
    Server(int port, QObject* parent = 0);
    ~Server();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    static Server* instance(int port);


private slots:
    void slotClientThreadError(QTcpSocket::SocketError socketError);
    void slotNewMessage(const QString& msg);
    void slotAddConnectedClient(const QString& client);
    void slotRemoveDisconnectedClient(const QString& client);

signals:
    void signalNewMessageClientThread(const QString& msg);

private:
    static Server* m_instance;
    quint16 m_nextBlockSize;
    QList<QString> m_clientList;
    QMutex m_clientListMutex;
};

#endif // SERVER_H
