#include "filewatcher.h"

FileWatcher::FileWatcher()
{
    watcher = new QFileSystemWatcher(this);
    ftp = new FtpClient();
    ftp->login();

    //获取Save目录
    save_path = QCoreApplication::applicationDirPath() + "/Save";
    qDebug() << "save path: " + save_path;
    dir = new QDir(save_path);
    if(!dir->exists()){
        dir->mkdir(save_path);
    }

    //获取Temp目录
    temp_path = QCoreApplication::applicationDirPath() + "/Temp/save.dat";
    QDir *temp_dir = new QDir(QCoreApplication::applicationDirPath() + "/Temp");
    QFile *data_file = new QFile(temp_path);

    //检查temp目录是否存在
    if(!temp_dir->exists()){
        temp_dir->mkdir(QCoreApplication::applicationDirPath() + "/Temp");
    }

    if(!data_file->exists()){
        if(!data_file->open(QIODevice::WriteOnly)){
            qDebug() << "创建文件失败.";
        }
    }else{
        if(!data_file->open(QIODevice::ReadOnly)){
            qDebug() << "读取文件失败.";
        }
        //读取数据到等待队列
        while(!data_file->atEnd()){
            file_wait_list.append(data_file->readLine());
        }
    }

    //保存当前目录下的文件，注意需使用绝对路径
    dir->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    file_list = getFileFullPath(dir->entryList());

    //开启目录监控
    watcher->addPath(save_path);
    watcher->addPaths(file_list);

    //启动线程
    start();

    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &FileWatcher::onDirChanged);
    connect(watcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::onFileChanged);
    connect(ftp, &FtpClient::fileError, this, &FileWatcher::onFileError);

    data_file->close();
    delete temp_dir;
    delete data_file;
}

FileWatcher::~FileWatcher()
{
    stopThread();
    saveFailedFile();
    delete watcher;
    ftp->deleteLater();
}

//处理文件更新
void FileWatcher::onFileChanged(const QString &path)
{
    //判断变动是否为删除（检查变动文件是否存在）
    QFile *file = new QFile(path);
    if(!file->exists()){
        delete file;
        return;
    }
    qDebug() << "文件更新";
    qDebug() << "File: " + path + " Changed";

    //上传文件
    ftp->uploadFile(path);
}

//处理目录更新
void FileWatcher::onDirChanged(const QString &)
{
    dir->refresh();
    QMap<int, QStringList> map = compareFileList(
                getFileFullPath(dir->entryList()),
                file_list);

    //文件增加
    if(map.find(ADD).key() == ADD){
        qDebug() << "文件增加";
        //上传文件        
        ftp->uploadFile(map[ADD][0]);
        qDebug() << "Add File: " + map[ADD][0];
        //更新目录和监控目录
        file_list.append(map[ADD]);
        watcher->addPaths(map[ADD]);
    }

    //文件减少
    if(map.find(SUB).key() == SUB){
        qDebug() << "文件减少";
        qDebug() << "Remove File: " + map[SUB][0];

        //更新目录
        foreach(const QString &it, map[SUB]){
            file_list.removeOne(it);
            watcher->removePath(it);
        }        
    }

    //文件更新(变名)
    if(map.find(EQUAL).key() == EQUAL){
        QStringList list = map[EQUAL];
        //若未更新文件名，则此处不处理
        if(list.isEmpty())    return;

        qDebug() << "文件变名";
        qDebug() << list[1] + " -> " + list[0];

        ftp->uploadFile(list[0]);
        //移除旧文件名
        file_list.removeOne(list[1]);
        watcher->removePath(list[1]);

        //新增新文件名
        file_list.append(list[0]);
        watcher->addPath(list[0]);
    }
}

void FileWatcher::onFileError(int error, const QString &path)
{
    //处理被占用的文件
    if(error == FILE_CANNOT_READ){
        file_wait_list.append(path);
    }
    //处理上传失败的文件
    if(error == FILE_UPDATE_FAILED){
        file_failed_list.append(path);
    }
}

//多线程任务，用于处理file_wait_list
void FileWatcher::run()
{
    while(!isStop){
        mutex.lock();
        if(!file_wait_list.isEmpty()){
            foreach(const QString &it, file_wait_list){
                ftp->uploadFile(it);
                file_wait_list.removeOne(it);
            }
        }
        mutex.unlock();
        msleep(100);
    }
}

//保存未上传成功的文件
void FileWatcher::saveFailedFile()
{
    QFile *file = new QFile(temp_path);
    if(!file->open(QIODevice::WriteOnly)){
        qDebug() << "读取失败.";
        return;
    }

    QTextStream stream(file);
    stream.seek(file->size());
    foreach(const QString &it, file_wait_list){
        stream << it + "\n";
    }

    foreach(const QString &it, file_failed_list){
        stream << it + "\n";
    }
    file->close();
    delete file;
}

void FileWatcher::stopThread()
{
    isStop = true;
    wait();
}
/*
 * 比较两个目录，用于检查原目录是否发生更新，返回QMap<int, QStringList>
 * 如果int为EQUAL(0)，则表示目录存在文件名变化或文件被覆盖，QStringList为空
 * 如果int为ADD(1)，则表示目录文件增加，QStringList将存储增加的元素
 * 如果int为SUB(-1)，则表示目录文件减少，QStringList将存储减少的元素
*/
QMap<int, QStringList> FileWatcher::compareFileList(const QStringList &new_dir, const QStringList &old_dir)
{
    QMap<int, QStringList> map;
    QStringList list;    

    /* 相等(文件更新)
     * 文件更新分为两种情况：
     * 1. 文件变名
     * 2. 文件被覆盖或被修改
     *
     * 当文件变名时，fileChanged将失效，此时需找出改变的文件名并返回.
     * 返回格式如下：
     * map<EQUAL, {new_filename, old_filename}>
     *
     * 当文件被覆盖或被修改时，则不处理此响应，返回空list.
    */
    if(new_dir.size() == old_dir.size()){        
        //查找文件新文件名
        foreach(const QString &it, new_dir){
            if(!old_dir.contains(it)){
                list.append(it);
            }
        }
        //查找旧文件名
        foreach(const QString &it, old_dir){
            if(!new_dir.contains(it)){
                list.append(it);
            }
        }
        map.insert(EQUAL, list);
        return map;
    }

    //文件增加
    if(new_dir.size() > old_dir.size()){
        foreach(const QString &it, new_dir){
            if(old_dir.contains(it)){
                continue;
            }
            list.append(it);
        }
        map.insert(ADD, list);
        return map;
    }

    //文件减少
    foreach(const QString &it, old_dir){
        if(new_dir.contains(it)){
            continue;
        }
        list.append(it);
    }
    map.insert(SUB, list);
    return map;
}

//获取文件绝对路径
QStringList FileWatcher::getFileFullPath(const QStringList &file_name) const
{
    QStringList full_path;
    foreach(QString it,file_name){
        full_path.append(QString(save_path + "/" + it));
    }
    return full_path;
}

QString FileWatcher::getFileFullPath(const QString &file_name) const
{
    return QString(save_path + "/" + file_name);
}
