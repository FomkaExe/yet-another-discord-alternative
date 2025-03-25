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
    void sendToClient(const QString& message, QTcpSocket* sock);
    void sendToAllClients(const QString& message);

private slots:
    void slotNewConnection();
    void slotReadClient();
    void slotClientDisconnected();

private:
    static Server* m_instance;
    QTcpServer* m_tcpServer;
    quint16 m_nextBlockSize;
    QMap<QTcpSocket*, QString> m_clientList;
};

#endif // SERVER_H
