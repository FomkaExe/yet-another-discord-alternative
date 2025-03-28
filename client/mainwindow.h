#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QListWidget>

#include "client.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initConnectWindow();
    void initMenuBar();
    void initMainLayout();

private slots:
    void slotSendMessage();
    void slotClientConnect(const QString& address, const QString& nickname);
    void slotUpdateClientList(const QStringList& clientsList);

private:
    /* Widgets */
    QWidget* m_centralWidget;
    QLabel* m_programNameLabel;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_chatClientListLayout;
    QTextEdit* m_chatWindow;
    QListWidget* m_clientListWindow;
    QLineEdit* m_txtInput;
    QPushButton* m_sendButton;
    QMenuBar* m_menuBar;
    QWidget* m_connectWindow;

    /* Client logic */
    Client* m_client;
    QString m_serverAddress;
};
#endif // MAINWINDOW_H
