#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#define ADD 1
#define SUB -1
#define EQUAL 0

#include <QObject>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QThread>
#include <QMutex>
#include "ftpclient.h"

#define ILLEGAL_PATH 1
#define FILE_CANNOT_READ 2
#define FILE_UPDATE_FAILED 3
#define FILE_UPDATE_SUCCESS 0

//监控Save目录并实时上传目录下的文件
class FileWatcher : public QThread
{
    Q_OBJECT
public:
    FileWatcher();
    ~FileWatcher();
    QMap<int, QStringList> compareFileList(QStringList const &new_dir, QStringList const &old_dir);
    QStringList getFileFullPath(const QStringList &file_name) const;
    QString getFileFullPath(const QString &file_name) const;
    void run() override;
    void stopThread();
    void saveFailedFile();

private slots:
    //处理FileSystemWatcher发出的fileChanged信号
    void onDirChanged(const QString &path);
    void onFileChanged(const QString &path);
    void onFileError(int error, const QString &path);

private:
    QFileSystemWatcher *watcher;    
    QStringList file_list;
    QStringList file_wait_list;
    QStringList file_failed_list;

    QDir *dir;
    QString save_path;
    QString temp_path;
    FtpClient *ftp;
    QMutex mutex;
    bool isStop = false;
};


#endif // FILEWATCHER_H
