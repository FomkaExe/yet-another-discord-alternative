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
    void slotShowMessage(const QString& message);
    void slotClientConnect(const QString& address);
    void slotClientDisconnect();

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
};
#endif // MAINWINDOW_H
