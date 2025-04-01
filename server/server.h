#ifndef SERVER_H
#define SERVER_H
#include "clientthread.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

class Server : public QTcpServer {
    Q_OBJECT

private:
    Server(int port, QObject* parent = 0);
    ~Server();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public:
    static Server* instance(int port);
    // void sendToClient(const QString& message, QTcpSocket* sock);
    // void sendToAllClients(const QString& message);
    // void updateClientList();

private slots:
    void slotClientThreadError(QTcpSocket::SocketError socketError);
    void slotNewMessage(QString msg);
    void slotClientListUpdated(QString client);

    // void slotReadClient();
    // void slotClientDisconnected();

signals:
    void signalNewMessageClientThread(QString msg);

private:
    static Server* m_instance;
    quint16 m_nextBlockSize;
    QMap<ClientThread*, QString> m_clientList;
};

#endif // SERVER_H
