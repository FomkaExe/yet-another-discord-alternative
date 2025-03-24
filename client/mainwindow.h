#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

#include "client.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slotSendMessage();
    void slotShowMessage(const QString& message);

private:
    QWidget* m_centralWidget;
    QVBoxLayout* m_layout;
    QLabel* m_programNameLabel;
    QTextEdit* m_txtInfo;
    QLineEdit* m_txtInput;
    QPushButton* m_sendButton;

    Client* m_client;
};
#endif // MAINWINDOW_H
