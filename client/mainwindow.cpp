#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_centralWidget(new QWidget(this)),
    m_layout(new QVBoxLayout(m_centralWidget)),
    m_programNameLabel(new QLabel("<H1>Client</H1>", m_centralWidget)),
    m_txtInfo(new QTextEdit(m_centralWidget)),
    m_txtInput(new QLineEdit(m_centralWidget)),
    m_sendButton(new QPushButton("Send", m_centralWidget)),
    m_client(new Client()) {
    setCentralWidget(m_centralWidget);

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

    m_client->connectToServer("127.0.0.1", 23012);
}

MainWindow::~MainWindow() {
    delete m_sendButton;
    delete m_txtInput;
    delete m_txtInfo;
    delete m_programNameLabel;
    delete m_layout;
    delete m_centralWidget;
}

void MainWindow::slotSendMessage() {
    QString message = m_txtInput->text();
    m_client->sendToServer(message);
    m_txtInput->clear();
}

void MainWindow::slotShowMessage(const QString& message) {
    m_txtInfo->append(message);
}
