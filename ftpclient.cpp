#include "ftpclient.h"

FtpClient::FtpClient()
{
    ftp = new QFtp(this);
    conn_timer = new QTimer();
    upload_timer = new QTimer();

    //ftp
    connect(ftp, &QFtp::commandStarted, this, &FtpClient::onCommandStarted);
    connect(ftp, &QFtp::commandFinished, this, &FtpClient::onCommandFinished);

    //timer
    connect(conn_timer, &QTimer::timeout, this, &FtpClient::onLoginTimeOut);
    connect(upload_timer, &QTimer::timeout, this, &FtpClient::onUploadTimeOut);

    //error
    connect(this, &FtpClient::connectError, this, &FtpClient::onConnectError);
    connect(this, &FtpClient::loginError, this, &FtpClient::onLoginError);
    connect(this, &FtpClient::uploadError, this, &FtpClient::onUploadError);
    connect(this, &FtpClient::downloadError, this, &FtpClient::onDownloadError);
}

FtpClient::~FtpClient()
{
    if(ftp->state() == QFtp::Connected){
        ftp->close();
    }
    delete ftp;
}

void FtpClient::login()
{    
    //登录主机
    ftp->connectToHost("10.23.87.19", 21);
    ftp->login("niko", "root123");

    //设置被动模式
    ftp->setTransferMode(QFtp::Passive);
    ftp->cd("Save");
}

//文件上传
void FtpClient::uploadFile(const QString &src_path)
{
    QByteArray byte_file;

    if(src_path.isEmpty()){
        qDebug() << QString::fromLocal8Bit("上传文件路径为空.");
        emit fileError(ILLEGAL_PATH, QString());
        return;
    }

    m_file = new QFile(src_path);
    if(!m_file->exists()){        
        qDebug() << QString::fromLocal8Bit("上传文件不存在.");
        emit fileError(ILLEGAL_PATH, QString());
        return;
    }

    //上传文件
    //在文件上传过程中可能出现文件被占用的情况，此时应将文件放入等待区，待文件解除占用后上传
    if(!m_file->open(QIODevice::ReadOnly)){        
        qDebug() << QString::fromLocal8Bit("读取文件失败，当前文件正在被占用");
        emit fileError(FILE_CANNOT_READ, src_path);
        return;
    }

    now_file = src_path;
    byte_file = m_file->readAll();
    ftp->put(byte_file, getFileName());
    m_file->close();
}

//文件下载
void FtpClient::downloadFile(const QString &path, const QString &file_name)
{
    if(path.isEmpty() || file_name.isEmpty()){
        qDebug() << QString::fromLocal8Bit("文件路径为空.");
        return;
    }

    m_file = new QFile();
    m_file->setFileName(file_name);
    if(!m_file->open(QIODevice::WriteOnly)){
        qDebug() << QString::fromLocal8Bit("写入文件失败.");
        return;
    }
    ftp->cd(path);
    ftp->get(file_name, m_file);
}

QString FtpClient::getFileName() const
{
    return QFileInfo(*m_file).fileName();
}

void FtpClient::onCommandStarted(int)
{
    //连接主机
    //在连接主机后启动计时器，当超过2s未连接到主机将进入交由onTimerOut处理
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandStarted => 尝试连接主机...");
        conn_timer->start(MAX_CONN_TIME_OUT);
    }

    //登录
    if(ftp->currentCommand() == QFtp::Login){
        qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandStarted => 正在登录...");
    }

    //下载
    if(ftp->currentCommand() == QFtp::Get){
        qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandStarted => 正在下载文件...");
    }

    //上传
    if(ftp->currentCommand() == QFtp::Put){
        qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandStarted => 正在上传文件...");
        upload_timer->start(MAX_UPLOAD_TIME_OUT);
    }

    //中断连接
    if(ftp->currentCommand() == QFtp::Close){
        qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandStarted => 正在中断连接...");
    }
}

void FtpClient::onCommandFinished(int, bool error)
{
    if(ftp->currentCommand() == QFtp::ConnectToHost){
        if(error){
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 连接主机失败.");
        }else{
            conn_timer->stop();
            connect_error_count = 0;
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 连接主机成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Login){
        if(error){
            qDebug()<< QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 登录失败.");
            login_error_count++;
            //检查登录错误次数
            if(login_error_count >= MAX_ERROR_COUNT){
                qDebug() << "无法处理Login Error.";
                QMessageBox::warning(NULL, QString::fromUtf8("登录异常"), QString::fromUtf8("无法登录到服务器."));
                return;
            }
            emit loginError();
        }else{
            login_error_count = 0;
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 登录成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Get){
        if(error){
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 下载失败.");
        }else{
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 下载成功.");
        }
        m_file->close();
    }

    if(ftp->currentCommand() == QFtp::Put){
        if(error){
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 上传失败.");
        }else{
            upload_timer->stop();
            upload_error_count = 0;
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 上传成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Cd){
        if(error){
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 切换目录失败.");
        }else{
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 切换目录成功.");
        }
    }

    if(ftp->currentCommand() == QFtp::Close){
        if(error){
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 中断连接失败.");
        }else{
            qDebug() << QString::fromLocal8Bit("FtpClient::ftpCommandFinished => 终端连接成功.");
        }
    }
}

void FtpClient::onConnectError()
{
    qDebug() << QString::fromLocal8Bit("无法处理Connect Error.");
    ftp->close();
    return;
}


void FtpClient::onLoginError()
{
    //尝试重新登录
    login();
}

void FtpClient::onUploadError()
{
    //处理上传异常
    emit fileError(FILE_UPDATE_FAILED, now_file);
}

void FtpClient::onDownloadError()
{

}

void FtpClient::onLoginTimeOut()
{
    qDebug() << QString::fromLocal8Bit("连接超时.");
    conn_timer->stop();
    emit onConnectError();
}

void FtpClient::onUploadTimeOut()
{
    if(ftp->state() == QFtp::Unconnected){
        qDebug() << QString::fromLocal8Bit("与主机断开连接，上传失败.");
        upload_timer->stop();
        emit uploadError();
    }
}
