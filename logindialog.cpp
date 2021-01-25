#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    NetworkCookieJar jar;
    qDebug() << jar.getCookies();
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text().trimmed();
    if (username.isEmpty() || password.isEmpty())
        return ;

    if (ui->neteaseRadio->isChecked())
    {
        loginNetease(username, password);
    }
    else if (ui->qqmusicRadio->isChecked())
    {
        loginQQMusic(username, password);
    }
}

void LoginDialog::loginNetease(QString username, QString password)
{
    password = password.toLocal8Bit().toPercentEncoding();
    bool usePhone = (username.indexOf("@") == -1);
    QString url = NETEASE_SERVER + (usePhone
            ? "/login/cellphone?phone=" + username +"&password=" + password
            : "/login?email=" + username + "&password=" + password);

    QNetworkAccessManager* manager = new QNetworkAccessManager;
    QNetworkRequest* request = new QNetworkRequest(url);
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
    request->setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.111 Safari/537.36");
    connect(manager, &QNetworkAccessManager::finished, this, [=](QNetworkReply* reply){
        QByteArray baData = reply->readAll();
        if(reply->hasRawHeader("Set-Cookie"))
        {
            QByteArray cookie = reply->rawHeader("Set-Cookie");
            emit signalLogined(NeteaseCloudMusic, cookie);
        }

        manager->deleteLater();
        delete request;
        reply->deleteLater();
        this->close();
    });
    manager->get(*request);
}

void LoginDialog::loginQQMusic(QString username, QString password)
{

}
