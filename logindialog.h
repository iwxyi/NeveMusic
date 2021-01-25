#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "songbeans.h"

namespace Ui {
class LoginDialog;
}

#define NETEASE_SERVER QString("http://iwxyi.com:3000")
#define QQMUSIC_SERVER QString("http://iwxyi.com:3200")

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void on_loginButton_clicked();

private:
    void loginNetease(QString username, QString password);
    void loginQQMusic(QString username, QString password);

signals:
    void signalLogined(MusicSource source, QString cookies);

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
