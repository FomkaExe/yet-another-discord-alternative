#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>

#include "client.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initConnectWindow();
    void initMenuBar();

private slots:
    void slotSendMessage();
    void slotClientConnect(const QString& address, const QString& nickname);

private:
    /* Widgets */
    QWidget* m_centralWidget;
    QVBoxLayout* m_layout;
    QLabel* m_programNameLabel;
    QTextEdit* m_chatWindow;
    QLineEdit* m_txtInput;
    QPushButton* m_sendButton;
    QMenuBar* m_menuBar;
    QWidget* m_connectWindow;

    /* Client logic */
    Client* m_client;
    QString m_serverAddress;
};
#endif // MAINWINDOW_H
