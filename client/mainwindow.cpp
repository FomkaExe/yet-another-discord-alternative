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
    QLabel* addressLabel = new QLabel("Server address:");
    QLineEdit* serverAddressLineEdit = new QLineEdit(m_serverAddress);
    QPushButton* connectBtn = new QPushButton("Connect");
    QPushButton* cancelBtn = new QPushButton("Cancel");

    QVBoxLayout* masterLayout = new QVBoxLayout(m_connectWindow);
    QHBoxLayout* hButtonsLayout = new QHBoxLayout();

    hButtonsLayout->addWidget(connectBtn);
    hButtonsLayout->addWidget(cancelBtn);
    masterLayout->addWidget(addressLabel);
    masterLayout->addWidget(serverAddressLineEdit);
    masterLayout->addLayout(hButtonsLayout);

    m_connectWindow->setWindowModality(Qt::ApplicationModal);

    connect(serverAddressLineEdit, &QLineEdit::returnPressed,
            this, [=](){m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress);
                        m_connectWindow->close();});

    connect(connectBtn, &QAbstractButton::clicked,
            this, [=](){m_serverAddress = serverAddressLineEdit->text();
                        slotClientConnect(m_serverAddress);
                        m_connectWindow->close();});

    connect(cancelBtn, &QAbstractButton::clicked,
            this, [=](){m_connectWindow->close();});
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

void MainWindow::slotClientConnect(const QString& address) {
    if (address.isEmpty()) {
        statusBar()->showMessage("Error: address is empty");
        return;
    }
    QString ip = address.split(":").first();
    QString port = address.split(":").last();
    QStringList strList = address.split(":");
    if (strList.size() > 2) {
        /* address is standard ipv6 */
        strList.removeLast();
        ip = strList.join(":");
    }
    m_client->connectToServer(ip, port.toInt());
}
