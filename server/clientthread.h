#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <QThread>
#include <QTcpSocket>

class ClientThread : public QObject {
    Q_OBJECT
public:
    ClientThread(qintptr socketDescriptor);

signals:
    void signalError(QTcpSocket::SocketError socketError);
    void signalNewMessageServer(QString msg);
    void signalClientListUpdated(QString client);
    void signalFinished();

public slots:
    void start();
    void slotWriteClient(QString msg);

private slots:
    void slotReadClient();
    void slotClientDisconnected();

private:
    qintptr m_socketDescriptor;
    QThread* m_thread;
    quint16 m_nextBlockSize;
    QTcpSocket* m_tcpSocket;
    QString m_clientName;
};

#endif // CLIENTTHREAD_H
