#include "mainwindow.h"

#include <QDialog>
#include <QMessageBox>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_centralWidget(new QWidget(parent)),
    m_programNameLabel(new QLabel("<H1>Yet Another Discord Alternative</H1>",
                       m_centralWidget)),
    m_mainLayout(new QVBoxLayout(m_centralWidget)),
    m_chatClientListLayout(new QHBoxLayout()),
    m_chatWindow(new QTextEdit(m_centralWidget)),
    m_clientListWindow(new QListWidget()),
    m_txtInput(new QLineEdit(m_centralWidget)),
    m_sendButton(new QPushButton("Send", m_centralWidget)),
    m_menuBar(new QMenuBar(m_centralWidget)),
    m_statusBarLabel(new QLabel()),
    m_connectWindow(new QWidget(parent)),
    m_client(new Client()),
    m_serverAddress(QString("127.0.0.1:23012")),
    m_clientName(QString()) {
    setCentralWidget(m_centralWidget);
    initMenuBar();
    setMenuBar(m_menuBar);
    initConnectWindow();
    initMainLayout();
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
            this,   [=](){
                        QString name = nicknameLineEdit->text();
                        if (name.isEmpty()) {
                            errorNicknameLabel->setText
                                ("Error: Nickname can't be empty");
                            m_clientName.clear();
                            return;
                        }
                        m_clientName = name;
                        errorNicknameLabel->clear();
                        m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress);
                        m_connectWindow->close();
                    });

    connect(nicknameLineEdit, &QLineEdit::returnPressed,
            this, [=](){
                        QString name = nicknameLineEdit->text();
                        if (name.isEmpty()) {
                            errorNicknameLabel->setText
                                ("Error: Nickname can't be empty");
                            m_clientName.clear();
                            return;
                        }
                        m_clientName = name;
                        errorNicknameLabel->clear();
                        m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress);
                        m_connectWindow->close();
                    });

    connect(connectBtn, &QAbstractButton::clicked,
            this,   [=](){
                        QString name = nicknameLineEdit->text();
                        if (name.isEmpty()) {
                            errorNicknameLabel->setText
                                ("Error: Nickname can't be empty");
                            m_clientName.clear();
                            return;
                        }
                        m_clientName = name;
                        errorNicknameLabel->clear();
                        m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress);
                        m_connectWindow->close();
                    });

    connect(cancelBtn, &QAbstractButton::clicked,
            this,   [=](){
                        errorNicknameLabel->clear();
                        m_clientName.clear();
                        m_connectWindow->close();
                    });
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
            this,   [this]() {
                        m_connectWindow->show();
                    });

    connect(disconnectAct, &QAction::triggered,
            this,   [this](){
                        m_client->disconnectFromServer();
                        m_chatWindow->clear();
                        m_clientName.clear();
                        slotUpdateClientList(QStringList());
                        m_statusBarLabel->setText("Disconnected");
                    });

    connect(exitAct, &QAction::triggered,
            this,   [this](){
                        this->close();
                    });

    connect(aboutQtAct, &QAction::triggered,
            this,   [this](){
                        QMessageBox::aboutQt(m_centralWidget);
                    });
}

void MainWindow::initMainLayout() {
    statusBar()->addWidget(m_statusBarLabel);

    m_chatWindow->setReadOnly(true);

    m_chatClientListLayout->addWidget(m_chatWindow);
    m_chatClientListLayout->addWidget(m_clientListWindow);
    m_chatClientListLayout->setStretch(0, 2);
    m_chatClientListLayout->setStretch(1, 1);
    m_mainLayout->addWidget(m_programNameLabel);
    m_mainLayout->addLayout(m_chatClientListLayout);
    m_mainLayout->addWidget(m_txtInput);
    m_mainLayout->addWidget(m_sendButton);
    m_centralWidget->setLayout(m_mainLayout);

    connect(m_sendButton, &QAbstractButton::clicked,
            this, &MainWindow::slotSendMessage);
    connect(m_txtInput, &QLineEdit::returnPressed,
            this, &MainWindow::slotSendMessage);

    connect(m_client, &Client::readyReadSuccess,
            this,   [=](const QString& message){
                m_chatWindow->append(message);
            });

    connect(m_client, &Client::connectedToServer,
            this,   [=](){
                m_statusBarLabel->setText("Connected to server");
            });

    connect(m_client, &Client::signalSocketError,
            this,   [=](QString socketError) {
                m_statusBarLabel->setText(socketError);
                slotUpdateClientList(QStringList());
            });

    connect(m_client, &Client::clientListUpdated,
            this, &MainWindow::slotUpdateClientList);
}

void MainWindow::slotSendMessage() {
    QString message = m_txtInput->text();
    m_client->sendToServer(message);
    m_txtInput->clear();
}

void MainWindow::slotClientConnect(const QString& address) {
    if (address.isEmpty()) {
        m_statusBarLabel->setText("Error: address is empty");
        return;
    }

    /* default case is ipv4 without port */
    QStringList addressParts = address.split(':');
    QString ip = addressParts.first();
    QString port = "23012";

    if (addressParts.size() == 2) {
        /* ipv4 with port */
        ip = addressParts.first();
        port = addressParts.last();
    } else {
        if (addressParts.at(0).at(0) == "[") {
            /* ipv6 with port */
            port = addressParts.last();
            addressParts.removeLast();
            ip = addressParts.join(':');
            ip.removeLast();
            ip.removeFirst();
            qDebug() << "ip: " << ip << '\n' << "port: " << port;
        } else {
            /* ipv6 without port */
            ip = addressParts.join(':');
            port = "23012";
            qDebug() << "ip: " << ip << '\n' << "port: " << port;
        }
    }
    m_client->connectToServer(ip, port.toInt(), m_clientName);
}

void MainWindow::slotUpdateClientList(const QStringList& clientsList) {
    m_clientListWindow->clear();
    for (auto it = clientsList.begin(); it != clientsList.end(); ++it) {
        QListWidgetItem* item = (*it == m_clientName ?
                                new QListWidgetItem(QIcon(":/icons/userRed"), *it) :
                                new QListWidgetItem(QIcon(":/icons/userBlack"), *it));
        m_clientListWindow->addItem(item);
    }
}
