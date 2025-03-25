#include "mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_centralWidget(new QWidget(parent)),
    m_layout(new QVBoxLayout(m_centralWidget)),
    m_programNameLabel(new QLabel("<H1>Client</H1>", m_centralWidget)),
    m_chatWindow(new QTextEdit(m_centralWidget)),
    m_txtInput(new QLineEdit(m_centralWidget)),
    m_sendButton(new QPushButton("Send", m_centralWidget)),
    m_menuBar(new QMenuBar(m_centralWidget)),
    m_connectWindow(new QWidget(parent)),
    m_client(new Client()),
    m_serverAddress(QString("127.0.0.1:23012")) {
    setCentralWidget(m_centralWidget);

    initMenuBar();
    setMenuBar(m_menuBar);
    initConnectWindow();

    connect(m_sendButton, &QAbstractButton::clicked,
            this, &MainWindow::slotSendMessage);
    connect(m_txtInput, &QLineEdit::returnPressed,
            this, &MainWindow::slotSendMessage);

    connect(m_client, &Client::readyReadSuccess,
            this, [=](const QString& message){m_chatWindow->append(message);});

    connect(m_client, &Client::connectedToServer,
            this, [=](){statusBar()->showMessage("Connected to server");});

    connect(m_client, &Client::signalSocketError,
            this, [=](QAbstractSocket::SocketError socketError) {
    statusBar()->showMessage(QString::number(static_cast<int>(socketError)));});

    m_chatWindow->setReadOnly(true);

    m_layout->addWidget(m_programNameLabel);
    m_layout->addWidget(m_chatWindow);
    m_layout->addWidget(m_txtInput);
    m_layout->addWidget(m_sendButton);
    m_centralWidget->setLayout(m_layout);
}

MainWindow::~MainWindow() {
    delete m_client;
}

void MainWindow::initConnectWindow() {
    QLabel* serverAddressLabel = new QLabel("Server address:");
    QLineEdit* serverAddressLineEdit = new QLineEdit(m_serverAddress);
    QLabel* nicknameLabel = new QLabel("Nickname: ");
    QLineEdit* nicknameLineEdit = new QLineEdit();
    QLabel* errorNicknameLabel = new QLabel();
    QPushButton* connectBtn = new QPushButton("Connect");
    QPushButton* cancelBtn = new QPushButton("Cancel");

    QVBoxLayout* masterLayout = new QVBoxLayout(m_connectWindow);
    QHBoxLayout* hButtonsLayout = new QHBoxLayout();

    hButtonsLayout->addWidget(connectBtn);
    hButtonsLayout->addWidget(cancelBtn);
    masterLayout->addWidget(serverAddressLabel);
    masterLayout->addWidget(serverAddressLineEdit);
    masterLayout->addWidget(nicknameLabel);
    masterLayout->addWidget(nicknameLineEdit);
    masterLayout->addWidget(errorNicknameLabel);
    masterLayout->addLayout(hButtonsLayout);

    m_connectWindow->setWindowModality(Qt::ApplicationModal);

    connect(serverAddressLineEdit, &QLineEdit::returnPressed,
            this, [=](){QString name = nicknameLineEdit->text();
                        if (name.isEmpty()) {
                            errorNicknameLabel->setText
                                ("Error: Nickname can't be empty");
                            return;
                        } else {
                            errorNicknameLabel->clear();
                        }
                        m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress, name);
                        m_connectWindow->close();});

    connect(connectBtn, &QAbstractButton::clicked,
            this, [=](){QString name = nicknameLineEdit->text();
                        if (name.isEmpty()) {
                            errorNicknameLabel->setText
                                ("Error: Nickname can't be empty");
                            return;
                        } else {
                            errorNicknameLabel->clear();
                        }
                        m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress, name);
                        m_connectWindow->close();});

    connect(cancelBtn, &QAbstractButton::clicked,
            this, [=](){errorNicknameLabel->clear();
                        m_connectWindow->close();});
}

void MainWindow::initMenuBar() {
    QMenu* connectMenu = new QMenu("Connect", m_menuBar);
    QMenu* aboutMenu = new QMenu("About", m_menuBar);
    QAction* connectAct = new QAction("Connect", connectMenu);
    QAction* disconnectAct = new QAction("Disconnect", connectMenu);
    QAction* exitAct = new QAction("Exit", connectMenu);
    QAction* aboutQtAct = new QAction("About qt", aboutMenu);

    m_menuBar->addMenu(connectMenu);
    m_menuBar->addMenu(aboutMenu);
    connectMenu->addAction(connectAct);
    connectMenu->addAction(disconnectAct);
    connectMenu->addAction(exitAct);
    aboutMenu->addAction(aboutQtAct);

    connect(connectAct, &QAction::triggered,
            this, [=](){m_connectWindow->show();});
    connect(disconnectAct, &QAction::triggered,
            this, [=](){m_client->disconnectFromServer();
                        m_chatWindow->clear();
                        statusBar()->showMessage("Disconnected");});
    connect(exitAct, &QAction::triggered,
            this, [=](){this->close();});
    connect(aboutQtAct, &QAction::triggered,
            this, [=](){QMessageBox::aboutQt(m_centralWidget);});
}

void MainWindow::slotSendMessage() {
    QString message = m_txtInput->text();
    m_client->sendToServer(message);
    m_txtInput->clear();
}

void MainWindow::slotClientConnect(const QString& address,
                                   const QString& nickname) {
    if (address.isEmpty()) {
        statusBar()->showMessage("Error: address is empty");
        return;
    }
    QStringList addressParts = address.split(':');
    QString ip = addressParts.first();
    QString port = "23012";
    if (addressParts.size() == 1) {
        /* ipv4 without port */
        qDebug() << "ipv4 without port";
        ip = addressParts.first();
        port = "23012";
    } else if (addressParts.size() == 2) {
        /* ipv4 with port */
        qDebug() << "ipv4 with port";
        ip = addressParts.first();
        port = addressParts.last();
    } else {
        /* ipv6 */
        if (addressParts.at(0).at(0) == "[") {
            /* ipv6 with port */
            qDebug() << "ipv6 with port";
            port = addressParts.last();
            addressParts.removeLast();
            ip = addressParts.join(':');
            ip.removeLast();
            ip.removeFirst();
            qDebug() << "ip: " << ip << '\n' << "port: " << port;
        } else {
            /* ipv6 without port */
            qDebug() << "ipv6 without port";
            ip = addressParts.join(':');
            port = "23012";
            qDebug() << "ip: " << ip << '\n' << "port: " << port;
        }
    }
    m_client->connectToServer(ip, port.toInt(), nickname);
}
