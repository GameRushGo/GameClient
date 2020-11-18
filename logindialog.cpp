#include "logindialog.h"
#include "ui_logindialog.h"
#include "cglobal.h"
#include <QMessageBox>
#include <QByteArray>
#include <QFile>
#include "structs.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    //设置窗口透明
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    //调用未公开api实现半透明效果
    HWND hwnd = HWND(this->winId());
    HMODULE hmoudle = GetModuleHandle(L"user32.dll");
    if(!hmoudle){
        qDebug() << "can not load user32.dll";
    }else{
        //获取api地址
        pfnSetWindowCompositionAttribute SetWindowCompositionAttribute = \
                (pfnSetWindowCompositionAttribute)GetProcAddress\
                (hmoudle, "SetWindowCompositionAttribute");
        if(SetWindowCompositionAttribute){
            ACCENT_POLICY accent = { ACCENT_ENABLE_BLURBEHIND, 0, 0, 0 };
            WINDOWCOMPOSITIONATTRIBDATA data;
            data.Attrib = WCA_ACCENT_POLICY;
            data.pvData = &accent;
            data.cbData = sizeof(accent);
            SetWindowCompositionAttribute(hwnd, &data);
        }
    }

    ui->setupUi(this);
    //隐藏菜单栏
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //设置显示模式为密码
    ui->passwd_lineEdit->setEchoMode(QLineEdit::Password);
    naManager = new QNetworkAccessManager(this);

    //====Test====
    ui->user_lineEdit->setText("17789435202");
    ui->passwd_lineEdit->setText("111111");
    //====Test====

    //响应取消按钮
    connect(ui->CancelButton, &QPushButton::pressed,
            [=]()
            {
                this->accept();
            }
            );

    //响应登录按钮
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
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));
}


LoginDialog::~LoginDialog()
{
    delete naManager;
    delete ui;
}

//验证登录
bool LoginDialog::login(const QString username, const QString password)
{
    //检查用户名和密码是否为空
    if(username.isEmpty() || password.isEmpty()){
        //设置全局登录标志为false
        isLogin = false;
        ui->LoginLabel->setText("用户名或密码不可为空");
        ui->LoginLabel->setStyleSheet("color:red;");
        ui->LoginButton->setEnabled(true);
        return false;
    }

    //发送get请求
    QString url = "http://localhost:8080/Game%20server%20manager_10_8_3/Judge_info?account=";
    url.append(username);
    url.append("&password=");
    url.append(password);
    request.setUrl(QUrl(url));
    qDebug() << url;
    QNetworkReply *reply = naManager->get(request);

    return true;
}

//处理异步请求
void LoginDialog::finishedSlot(QNetworkReply *reply)
{
    //处理报文信息
    QByteArray byte_array = reply->readAll();

    //恢复按钮可用
    ui->LoginButton->setEnabled(true);

    //检查回复报文是否存在错误
    if(reply->error() != QNetworkReply::NoError){
        //设置全局登录标志为false
        isLogin = false;
        qDebug() << reply->error();
        ui->LoginLabel->setText("网络连接异常，请重试");
        ui->LoginLabel->setStyleSheet("color:red;");       
        reply->deleteLater();
        return;
    }

    //检查返回数据是否为空
    if(byte_array.isEmpty() || byte_array.isNull()){
        isLogin = false;
        ui->LoginLabel->setText("用户名或密码错误");
        ui->LoginLabel->setStyleSheet("color:red;");
        reply->deleteLater();
        return;
    }

    //设置全局登录标志为true
    isLogin = true;
    ui->LoginLabel->setText("登录成功!");
    ui->LoginLabel->setStyleSheet("color:green;");
    reply->deleteLater();

    //读取json
    readJosn(byte_array);
    this->close();
}

//读取json数据
void LoginDialog::readJosn(const QByteArray &byte_array)
{
    QJsonParseError *json_error = new QJsonParseError();
    QJsonDocument *json_doc = new QJsonDocument(QJsonDocument::fromJson(byte_array, json_error));

    //检查json数据是否存在错误
    if(json_error->error != QJsonParseError::NoError){
        qDebug() << "Json Error.";
        return;
    }

    //读取value
    QJsonObject root_obj(json_doc->object());

    //type: int
    if(root_obj.contains("groupnum")){
        qDebug() << "groupnum: " + QString::number(root_obj["groupnum"].toInt());
        user_info.setGroupnum(root_obj["groupnum"].toInt());
    }

    //type: int
    if(root_obj.contains("research")){
        qDebug() << "research: " + QString::number(root_obj["research"].toInt());
        user_info.setResearch(root_obj["research"].toInt());
    }

    //type: QString
    if(root_obj.contains("name")){
        qDebug() << "name: " + root_obj["name"].toString();
        user_info.setName(root_obj["name"].toString());
        user_info.setName("唐婉");
    }

    //type: QString
    if(root_obj.contains("identity")){
        qDebug() << "identity: " + root_obj["identity"].toString();
        user_info.setIdentity(root_obj["identity"].toString());
        user_info.setIdentity("511864522214684435");
    }

    //type: QString
    if(root_obj.contains("tel")){
        qDebug() << "tel: " + root_obj["tel"].toString();
        user_info.setTel(root_obj["tel"].toString());
    }

    //type: QString
    if(root_obj.contains("department")){
        qDebug() << "department: " + root_obj["department"].toString();
        user_info.setDepartment(root_obj["department"].toString());
    }

    //type: QString
    if(root_obj.contains("grade")){
        qDebug() << "grade: " + root_obj["grade"].toString();
        user_info.setGrade(root_obj["grade"].toString());
    }

    //type: float
    if(root_obj.contains("weight")){
        qDebug() << "weight: " + QString::number(root_obj["weight"].toDouble());
        user_info.setWeight(root_obj["weight"].toDouble());
    }

    //type: QString
    if(root_obj.contains("vision")){
        qDebug() << "vision: " + root_obj["vision"].toString();
        user_info.setVision(root_obj["vision"].toString());
    }

    //type: QString
    if(root_obj.contains("account")){
        qDebug() << "account: " + root_obj["account"].toString();
        user_info.setAccount(root_obj["account"].toString());
    }

    delete json_doc;
    delete json_error;
}
