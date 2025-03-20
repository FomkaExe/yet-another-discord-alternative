#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject {
    Q_OBJECT

private:
    Server(int port);
    ~Server();

public:
    static Server* instance(int port);
    void sendToClient(QString& message, QTcpSocket* sock);
    void sendToAllClients(QString& message);

private slots:
    void slotNewConnection();
    void slotReadClient();
    void slotClientDisconnected();

private:
    static Server* m_instance;
    QTcpServer* m_tcpServer;
    quint16 m_nextBlockSize;
    QList<QTcpSocket*> m_clientList;
};

#endif // SERVER_H
