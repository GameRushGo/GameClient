#ifndef FTPCLIENT_H
#define FTPCLIENT_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QFtp>
#include <QFileInfo>
#include <QMessageBox>
#include <QTimer>

#define MAX_ERROR_COUNT 3
#define MAX_CONN_TIME_OUT 3000
#define MAX_UPLOAD_TIME_OUT 5000

#define ILLEGAL_PATH 1
#define FILE_CANNOT_READ 2
#define FILE_UPDATE_FAILED 3
#define FILE_UPDATE_SUCCESS 0


//FTP封装
class FtpClient : public QObject
{
    Q_OBJECT
public:
    FtpClient();
    ~FtpClient();

    void login();
    //上传和下载文件
    void uploadFile(const QString &src_path);
    void downloadFile(const QString &path, const QString &file_name);
    //获取文件名
    QString getFileName() const;

private Q_SLOTS:
    void onCommandStarted(int);
    void onCommandFinished(int, bool);
    void onConnectError();
    void onLoginError();
    void onUploadError();
    void onDownloadError();
    void onLoginTimeOut();
    void onUploadTimeOut();

//错误信号：连接失败，登录失败，上传失败，下载失败
signals:
    void connectError();
    void loginError();
    void fileError(int, const QString &path);
    void uploadError();
    void downloadError();    

private:
    QString now_file;
    QFile *m_file;
    QFtp *ftp;
    QTimer *conn_timer;
    QTimer *upload_timer;

    //错误计数器，表示最大重试次数，当错误次数达到MAX_ERROR_COUNT(最大错误次数)时，将终止尝试
    int connect_error_count = 0;
    int login_error_count = 0;
    int upload_error_count = 0;
    int download_error_count = 0;
};

#endif // FTPCLIENT_H
