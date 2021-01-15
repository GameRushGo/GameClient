#include "logindialog.h"
#include "ui_logindialog.h"
#include "cglobal.h"
#include "structs.h"
#include "rc4.h"
#include "main.cpp"

#include <QMessageBox>
#include <QByteArray>
#include <string>
#include <QFile>

////////////////////////
//#define NotNeedLogin
////////////////////////

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    //初始化类
    m_data = new ManageData();
    naManager = new QNetworkAccessManager(this);
    ui->setupUi(this);   

    //隐藏菜单栏
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    //设置显示模式为密码
    ui->passwd_lineEdit->setEchoMode(QLineEdit::Password);

    //读取本地密码信息
    readPasswd();
    //绑定取消按钮
    connect(ui->CancelButton, &QPushButton::pressed,
            [=]()
            {
                this->accept();
            }
            );

    //绑定登录按钮
    connect(ui->LoginButton, &QPushButton::pressed,
            [=]()
            {
                //获取账户名和密码
                QString username = ui->user_lineEdit->text();
                QString password = ui->passwd_lineEdit->text();

                //按钮置灰
                ui->LoginButton->setEnabled(false);

                //以get方式发送url请求校验账户名和密码，并接收登录成功与否信息
                login(username, password);
            }
            );

    //绑定web请求
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinishedSlot(QNetworkReply*)));
}


LoginDialog::~LoginDialog()
{
    delete naManager;
    delete ui;
}

//验证登录
bool LoginDialog::login(const QString& username, const QString& password)
{
#ifdef NotNeedLogin
    isLogin = true;

    this->accept();
    return true;

#else
    QString enPassword;
    QString url;

    //获取url
    url = m_data->getLoginUrl();
    LOG(DEBUG) << url.toStdString();

    //检查用户名和密码是否为空
    if(username.isEmpty() || password.isEmpty()){
        //设置全局登录标志为false
        isLogin = false;        
        ui->InfoLabel->setText(QString::fromLocal8Bit("用户名或密码不可为空"));
        ui->LoginButton->setEnabled(true);
        return false;
    }

    //加密密码
    encryptData(password, enPassword);

    //发送get请求    
    url.append("?account=");
    url.append(username);
    url.append("&password=");
    url.append(enPassword);

    request.setHeader(QNetworkRequest::ContentTypeHeader, ("text/html; charset=utf-8"));
    request.setUrl(QUrl(url));

    LOG(DEBUG) << url.toStdString();   
    this->username = username;
    this->password = enPassword;

    QNetworkReply *reply = naManager->get(request);
    return true;
#endif
}

//处理异步请求
void LoginDialog::onfinishedSlot(QNetworkReply *reply)
{
    //处理报文信息
    QByteArray byte_array = reply->readAll();

    //恢复按钮可用
    ui->LoginButton->setEnabled(true);

    //检查回复报文是否存在错误
    if(reply->error() != QNetworkReply::NoError){
        //设置全局登录标志为false
        isLogin = false;
        LOG(ERROR) << QString(reply->error()).toStdString();
        ui->InfoLabel->setText(QString::fromLocal8Bit("网络连接异常，请重试."));
        reply->deleteLater();
        return;
    }

    //检查返回数据是否为空
    if(byte_array.isEmpty() || byte_array.isNull()){
        isLogin = false;
        ui->InfoLabel->setText(QString::fromLocal8Bit("服务器异常，请重试."));
        reply->deleteLater();
        return;
    }

    LOG(DEBUG) << byte_array.toStdString();

    //检查是否登录成功
    if(!QString::compare(QString(byte_array), "lgerror\r\n")){
        isLogin = false;
        ui->InfoLabel->setText(QString::fromLocal8Bit("用户名或密码错误，请重试."));
        reply->deleteLater();
        return;
    }


    //检查json数据是否合法
    if(!readJosn(byte_array)){
        isLogin = false;
        ui->InfoLabel->setText(QString::fromLocal8Bit("数据异常，请重试."));
        reply->deleteLater();
        return;
    }

    //FILE
    QFile *file = new QFile("test.json");
    if(!file->open(QIODevice::WriteOnly)){
        return;
    }
    file->write(byte_array);
    file->close();
    delete file;

    //保存文件
    savePasswd(this->password);

    //设置全局登录标志为true
    isLogin = true;

    reply->deleteLater();
    this->close();
}

//读取json数据
bool LoginDialog::readJosn(const QByteArray &byte_array)
{
    QJsonParseError *json_error = new QJsonParseError();
    QJsonDocument *json_doc = new QJsonDocument(QJsonDocument::fromJson(byte_array, json_error));

    //检查json数据是否存在错误
    if(json_error->error != QJsonParseError::NoError){
        LOG(DEBUG) << "Json Error.";
        return false;
    }

    //读取value
    QJsonObject root_obj(json_doc->object());

    //type: int
    if(root_obj.contains("groupnum")){        
        user_info.setGroupnum(root_obj["groupnum"].toInt());
    }

    //type: int
    if(root_obj.contains("research")){        
        user_info.setResearch(root_obj["research"].toInt());
    }

    //type: QString
    if(root_obj.contains("name")){        
        user_info.setName(root_obj["name"].toString());        
    }

    //type: QString
    if(root_obj.contains("identity")){        
        user_info.setIdentity(root_obj["identity"].toString());
    }

    //type: QString
    if(root_obj.contains("tel")){        
        user_info.setTel(root_obj["tel"].toString());
    }

    //type: QString
    if(root_obj.contains("department")){        
        user_info.setDepartment(root_obj["department"].toString());
    }

    //type: QString
    if(root_obj.contains("grade")){        
        user_info.setGrade(root_obj["grade"].toString());
    }

    //type: float
    if(root_obj.contains("weight")){        
        user_info.setWeight(root_obj["weight"].toDouble());
    }

    //type: QString
    if(root_obj.contains("vision")){        
        user_info.setVision(root_obj["vision"].toString());
    }

    //type: QString
    if(root_obj.contains("account")){        
        user_info.setAccount(root_obj["account"].toString());
    }

    delete json_doc;
    delete json_error;

    //emit finishedJson();
    return true;
}


//存储密码
/* 密码存储
 * 本函数将创建两个login.dat用于用户cookie.
 * 当用户正确登录后，将在程序所在同级目录和USERPROFILE目录创建login.dat文件
*/
void LoginDialog::savePasswd(const QString &password)
{    
    QString userProfilePath;
    QFile *localFile;
    QFile *backupFile;

    //本地保存
    localFile = new QFile("login.dat");
    if(!localFile->open(QIODevice::WriteOnly | QIODevice::Text)){
        LOG(ERROR) << "Create login.dat failed.";
        return;
    }       
    //写入文件
    localFile->write("account#");
    localFile->write(username.toUtf8());
    localFile->write("\npassword#");
    localFile->write(password.toUtf8());

    //USERPROFILE保存
    userProfilePath = QProcessEnvironment::systemEnvironment().value("USERPROFILE") + "\\login.dat";
    backupFile = new QFile(userProfilePath);

    if(!backupFile->open(QIODevice::WriteOnly | QIODevice::Text)){
        LOG(ERROR) << "Create login.dat failed.";
        return;
    }
    backupFile->write("account#");
    backupFile->write(username.toUtf8());
    backupFile->write("\npassword#");
    backupFile->write(password.toUtf8());

    if(localFile){
        localFile->close();
        delete localFile;
    }

    if(backupFile){
        backupFile->close();
        delete backupFile;
    }
}

//读取用户本地密码
void LoginDialog::readPasswd()
{
    QString passwd;
    QString account;

    //检查是否存在login.dat文件并读取
    QFile *file = new QFile("login.dat");
    if(!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        file->close();
        LOG(ERROR) << "Not find login file.";
        return;
    }

    QString account_line = file->readLine();
    //bug修复：对account_line是否为空进行验证
    //原bug：当account_line为空时，使用remove方法将触发异常
    if(account_line == ""){
        file->close();
        qDebug() << "Invaild Passwd File.";
        return;
    }
    account_line.remove(QChar(0xa));

    QString enpasswd_line = file->readLine();
    if(enpasswd_line == ""){
        file->close();
        qDebug() << "Invaild Passwd File.";
        return;
    }

    //分隔字符串
    account = account_line.split('#').at(1);
    QString enpasswd_list = enpasswd_line.split('#').at(1);

    //解密密码
    decryptData(enpasswd_list, passwd);

    ui->user_lineEdit->setText(account);
    ui->passwd_lineEdit->setText(passwd);

    //释放内存
    if(file){
        file->close();
        delete file;
    }
}

//加密数据
void LoginDialog::encryptData(const QString &in, QString &out)
{
    RC4 *rc4 = new RC4();
    unsigned char* in_text = new unsigned char[in.length() + 1];
    unsigned char* out_text = new unsigned char[2 * in.length() + 1];

    ZeroMemory(in_text, sizeof(unsigned char) * in.length() + 1);
    ZeroMemory(out_text, sizeof(unsigned char) * (2 * in.length() + 1));

    //字符串->unsigned char*
    memcpy(in_text, in.toLatin1().data(), in.length());

    //rc4加密
    rc4->init(this->key, this->key_len);
    rc4->crypt(in_text, out_text, in.length());
    out_text[in.length()] = '\0';

    //base64编码
    out = QString(QByteArray((char*)out_text).toBase64());

    //释放内存
    if(rc4)         delete rc4;
    if(in_text)     delete[] in_text;
    if(out_text)    delete[] out_text;

}

void LoginDialog::decryptData(const QString &in, QString &out)
{
    RC4 *rc4 = new RC4();
    unsigned char* in_text = new unsigned char[in.length() + 1];
    unsigned char* out_text = new unsigned char[in.length() + 1];

    ZeroMemory(in_text, sizeof(unsigned char) * (in.length() + 1));
    ZeroMemory(out_text, sizeof(unsigned char) * (in.length() + 1));

    //base64解码
    auto d_byte = QByteArray::fromBase64(in.toLatin1());
    memcpy(in_text, d_byte.data(), d_byte.length());

    //rc4解密
    rc4->init(this->key, this->key_len);
    rc4->crypt(in_text, out_text, d_byte.length());
    out_text[d_byte.length()] = '\0';

    //unsigned char* -> QString
    out = QString((char*)out_text);

    //内存释放
    if(in_text)     delete[] in_text;
    if(out_text)    delete[] out_text;
    if(rc4)         delete rc4;
}
