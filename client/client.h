#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer(const QString& ipv4, int port);
    void disconnectFromServer();
    qint64 sendToServer(const QString& message);

private slots:
    void slotErrorOccurred(QAbstractSocket::SocketError socketError);
    void slotReadyRead();
    void slotEmitConnected();

signals:
    void readyReadSuccess(const QString& message);
    void connectedToServer();

private:
    QTcpSocket* m_socket;
    quint16 m_nextBlockSize;
};

#endif // CLIENT_H
