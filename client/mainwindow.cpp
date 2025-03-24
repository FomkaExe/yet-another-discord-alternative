#include "mainwindow.h"

#include <QDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_centralWidget(new QWidget(parent)),
    m_layout(new QVBoxLayout(m_centralWidget)),
    m_programNameLabel(new QLabel("<H1>Client</H1>", m_centralWidget)),
    m_txtInfo(new QTextEdit(m_centralWidget)),
    m_txtInput(new QLineEdit(m_centralWidget)),
    m_sendButton(new QPushButton("Send", m_centralWidget)),
    m_menuBar(new QMenuBar(m_centralWidget)),
    m_connectMenu(new QMenu("Connect", m_menuBar)),
    m_aboutMenu(new QMenu("About", m_menuBar)),
    m_connectAct(new QAction("Connect", m_connectMenu)),
    m_disconnectAct(new QAction("Disconnect", m_connectMenu)),
    m_exitAct(new QAction("Exit", m_connectMenu)),
    m_aboutQtAct(new QAction("About qt", m_aboutMenu)),
    m_connectWindow(new QWidget(parent)),
    m_client(new Client()) {
    setCentralWidget(m_centralWidget);

    m_menuBar->addMenu(m_connectMenu);
    m_menuBar->addMenu(m_aboutMenu);
    m_connectMenu->addAction(m_connectAct);
    m_connectMenu->addAction(m_disconnectAct);
    m_connectMenu->addAction(m_exitAct);
    m_aboutMenu->addAction(m_aboutQtAct);

    setMenuBar(m_menuBar);

    connect(m_connectAct, &QAction::triggered,
            this, [=](){m_connectWindow->show();});
    connect(m_disconnectAct, &QAction::triggered,
            this, &MainWindow::slotClientDisconnect);
    connect(m_exitAct, &QAction::triggered,
            this, [=](){this->close();});
    connect(m_aboutQtAct, &QAction::triggered,
            this, [=](){QMessageBox::aboutQt(m_centralWidget);});

    connect(m_sendButton, &QAbstractButton::clicked,
            this, &MainWindow::slotSendMessage);
    connect(m_txtInput, &QLineEdit::returnPressed,
            this, &MainWindow::slotSendMessage);
    connect(m_client, &Client::readyReadSuccess,
            this, &MainWindow::slotShowMessage);

    m_txtInfo->setReadOnly(true);

    m_layout->addWidget(m_programNameLabel);
    m_layout->addWidget(m_txtInfo);
    m_layout->addWidget(m_txtInput);
    m_layout->addWidget(m_sendButton);
    m_centralWidget->setLayout(m_layout);

    initConnectWindow();
}

MainWindow::~MainWindow() {
    delete m_client;
}

void MainWindow::initConnectWindow() {
    QLabel* addressLabel = new QLabel("Server address:", m_connectWindow);
    QLineEdit* serverAddressLineEdit = new QLineEdit(m_connectWindow);
    QPushButton* connectBtn = new QPushButton("Connect", m_connectWindow);
    QPushButton* cancelBtn = new QPushButton("Cancel", m_connectWindow);

    QVBoxLayout* masterLayout = new QVBoxLayout(m_connectWindow);
    QVBoxLayout* vLineEditLayout = new QVBoxLayout(m_connectWindow);
    QHBoxLayout* hButtonsLayout = new QHBoxLayout(m_connectWindow);

    vLineEditLayout->addWidget(addressLabel);
    vLineEditLayout->addWidget(serverAddressLineEdit);
    hButtonsLayout->addWidget(connectBtn);
    hButtonsLayout->addWidget(cancelBtn);
    masterLayout->addLayout(vLineEditLayout);
    masterLayout->addLayout(hButtonsLayout);

    m_connectWindow->setWindowModality(Qt::ApplicationModal);

    connect(serverAddressLineEdit, &QLineEdit::returnPressed,
            this, [=](){slotClientConnect(serverAddressLineEdit->text());
                        m_connectWindow->close();});

    connect(connectBtn, &QAbstractButton::clicked,
            this, [=](){slotClientConnect(serverAddressLineEdit->text());
                        m_connectWindow->close();});

    connect(cancelBtn, &QAbstractButton::clicked,
            this, [=](){m_connectWindow->close();});
}

void MainWindow::slotSendMessage() {
    QString message = m_txtInput->text();
    m_client->sendToServer(message);
    m_txtInput->clear();
}

void MainWindow::slotShowMessage(const QString& message) {
    m_txtInfo->append(message);
}

void MainWindow::slotClientConnect(const QString& address) {
    if (address.isEmpty()) {
        return; // should throw error
    }
    if (!address.contains(':')) {
        return; // should throw error
    }
    QStringList ipPortList = address.split(":");
    if (ipPortList.size() > 2) {
        return; // should throw error
    }
    m_client->connectToServer(ipPortList.at(0), ipPortList.at(1).toInt());
}

void MainWindow::slotClientDisconnect() {
    m_client->disconnectFromServer();
}
