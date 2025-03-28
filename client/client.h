#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    void connectToServer(const QString& ipv4,
                         int port,
                         const QString& nickname);
    void disconnectFromServer();
    qint64 sendToServer(const QString& message);

private slots:
    void slotErrorOccurred(const QAbstractSocket::SocketError& socketError);
    void slotReadyRead();
    void slotEmitConnected();

signals:
    void readyReadSuccess(const QString& message);
    void connectedToServer();
    void signalSocketError(const QString& socketErrorString);
    void clientListUpdated(const QStringList& clientsList);

private:
    QTcpSocket* m_socket;
    quint16 m_nextBlockSize;
};

#endif // CLIENT_H
