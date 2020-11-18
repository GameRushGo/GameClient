#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    bool login(const QString username, const QString password);
    void writeInfo();
    //解析json数据
    void readJosn(const QByteArray &byte_array);

private slots:
    void finishedSlot(QNetworkReply *reply);

private:
    Ui::LoginDialog *ui;
    QNetworkRequest request;
    QNetworkAccessManager *naManager = new QNetworkAccessManager(this);

};

#endif // LOGINDIALOG_H
