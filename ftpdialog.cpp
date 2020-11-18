#include "ftpdialog.h"
#include "ui_ftpdialog.h"

FtpDialog::FtpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FtpDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    ftp = new QFtp(this);

    connect(ftp, &QFtp::commandStarted, this, &FtpDialog::ftpCommandStarted);
    connect(ftp, &QFtp::commandFinished, this, &FtpDialog::ftpCommandFinished);

    //绑定上传和下载按钮
    connect(ui->uploadButton, &QPushButton::pressed,
            [=]()
            {
                QString src = tr("D:/Qt_Project/GameRushGo/setting.ini");
                uploadFile(src);
            }
    );

    connect(ui->downloadButton, &QPushButton::pressed,
            [=]()
            {
                QString file_name = tr("readMe.txt");
                QString path = tr(".");
                downloadFile(path, file_name);
            }
    );
    login();
}

FtpDialog::~FtpDialog()
{
    ftp->close();
    ftp->deleteLater();

    delete ui;
}

void FtpDialog::login()
{
    ftp->connectToHost("47.94.39.239", 21);
    ftp->login("niko", "mq19990816");

    //设置被动模式
    ftp->setTransferMode(QFtp::Passive);
    ftp->cd("Save");
}

void FtpDialog::uploadFile(const QString &src_path)
{
    QByteArray byte_file;

    if(src_path.isEmpty()){
        qDebug() << "文件路径为空.";
        return;
    }
    m_file = new QFile(src_path);
    if(!m_file->exists()){
        qDebug() << "文件不存在.";
        return;
    }

    //上传文件
    if(!m_file->open(QIODevice::ReadOnly)){
        qDebug() << "读取文件失败.";
        return;
    }

    byte_file = m_file->readAll();
    ftp->put(byte_file, getFileName());

    m_file->close();
}

void FtpDialog::downloadFile(const QString &path, const QString &file_name)
{
    if(path.isEmpty() || file_name.isEmpty()){
        qDebug() << "文件路径为空.";
        return;
    }

    m_file = new QFile();
    m_file->setFileName(file_name);
    if(!m_file->open(QIODevice::WriteOnly)){
        qDebug() << "写入文件失败.";
        return;
    }
    ftp->cd(path);
    ftp->get(file_name, m_file);
}

QString FtpDialog::getFileName() const
{
    return QFileInfo(*m_file).fileName();
}

void FtpDialog::ftpCommandStarted(int)
{
    //连接主机
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        ui->textBrowser->append("FtpClient::ftpCommandStarted => 尝试连接主机...");
    }
    //登录
    if(ftp->currentCommand() == QFtp::Login){
        ui->textBrowser->append("FtpClient::ftpCommandStarted => 正在登录...");
    }
    //下载
    if(ftp->currentCommand() == QFtp::Get){
        ui->textBrowser->append("FtpClient::ftpCommandStarted => 正在下载文件...");
    }
    //上传
    if(ftp->currentCommand() == QFtp::Put){
        ui->textBrowser->append("FtpClient::ftpCommandStarted => 正在上传文件...");
    }
    //中断连接
    if(ftp->currentCommand() == QFtp::Close){
        ui->textBrowser->append("FtpClient::ftpCommandStarted => 正在中断连接...");
    }
}

void FtpDialog::ftpCommandFinished(int, bool error)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 连接主机失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 连接主机成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Login){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 登录失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 登录成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Get){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 下载失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 下载成功.");
        }
        m_file->close();
    }

    if(ftp->currentCommand() == QFtp::Put){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 上传失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 上传成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Cd){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 切换目录失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 切换目录成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Close){
        if(error){
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 中断连接失败.");
        }else{
            ui->textBrowser->append("FtpClient::ftpCommandFinished => 终端连接成功.");
        }
    }
}
