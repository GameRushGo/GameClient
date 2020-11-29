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
#include "managedata.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    bool login(const QString& username, const QString& password);
    void writeInfo();
    //解析json数据
    bool readJosn(const QByteArray &byte_array);
    //保存密码
    void savePasswd(const QString &password);
    //读取保存密码
    void readPasswd();

    //数据加密
    void encryptData(const QString &in, QString &out);
    void encryptData(const QByteArray &in, QByteArray &out);
    void encryptData(const QString &in, QByteArray &out);
    void encryptData(const QByteArray &in, QString &out);
    //数据解密
    void decryptData(const QString &in, QString &out);    

signals:
    void finishedJson();

private slots:
    void onfinishedSlot(QNetworkReply *reply);

private:
    Ui::LoginDialog *ui;
    QNetworkRequest request;
    QNetworkAccessManager *naManager = new QNetworkAccessManager(this);
    QString username;
    QString password;
    ManageData *m_data;

    const unsigned char* key = (unsigned char*)"A3tCf8bJ9ePs2LMI";
    long key_len = 16;
};

#endif // LOGINDIALOG_H
