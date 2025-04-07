#ifndef CLIENT_H
#define CLIENT_H

#include <QThread>
#include <QTcpSocket>

class Client : public QObject {
    Q_OBJECT
public:
    Client(qintptr socketDescriptor, QObject* parent = 0);

signals:
    void signalClientToServer(const QString& msg);
    void signalAddConnectedClient(const QString& client);
    void signalRemoveDisconnectedClient(const QString& client);

public slots:
    void slotClientToServer();
    void slotServerToClient(const QString& msg);

private slots:
    void slotDisconnected();

private:
    quint16 m_nextBlockSize;
    QTcpSocket m_client;
    QTcpSocket m_server;
    QString m_clientName;
};

#endif // CLIENT_H
