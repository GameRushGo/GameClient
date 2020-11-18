#ifndef FTPDIALOG_H
#define FTPDIALOG_H

#include <QDialog>
#include <QFtp>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

namespace Ui {
class FtpDialog;
}

class FtpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FtpDialog(QWidget *parent = nullptr);
    ~FtpDialog();
    void login();
    //上传和下载文件
    void uploadFile(const QString &src_path);
    void downloadFile(const QString &path, const QString &file_name);
    //获取文件名
    QString getFileName() const;

private Q_SLOTS:
    void ftpCommandStarted(int);
    void ftpCommandFinished(int, bool);

private:
    Ui::FtpDialog *ui;
    QFtp *ftp;
    QFile *m_file;
};

#endif // FTPDIALOG_H
