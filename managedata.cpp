#include "managedata.h"

ManageData::ManageData()
{
    //创建ini
    pIni = new QSettings(QCoreApplication::applicationDirPath() + "/setting.ini", QSettings::IniFormat);
    pIni->setIniCodec(QTextCodec::codecForName("utf-8"));
    //qDebug() << "setting.ini path: " << QCoreApplication::applicationDirPath() + "/setting.ini";
}

//回收内存
ManageData::~ManageData()
{
    delete pIni;
}

void ManageData::setDataPath(const QString &value)
{
    //写入path路径
    pIni->setValue("/Common/Path", value);
}

QString ManageData::getDataPath() const
{
    //从ini中读取配置路径
    QString path = pIni->value("/Common/Path").toString();

    //判断路径是否合法
    if(path.isEmpty()){
        return "";
    }

    //分离文件和根路径，只保留根目录
    int n = path.lastIndexOf("/");
    //qDebug() << path.left(n);
    return path.left(n);
}

void ManageData::setEEGPath(const QString &value)
{
    //写入EEG的path
    pIni->setValue("/EEG/Path", value);
}

QString ManageData::getEEGPath() const
{
    //从ini中读取配置路径
    QString path = pIni->value("/EEG/Path").toString();

    //判断路径是否合法
    if(path.isEmpty()){
        return "";
    }

    return path;
}

void ManageData::setBGPath(const QString &value)
{
    //写入BG的path
    pIni->setValue("/BG/Path", value);
}

QString ManageData::getBGPath() const
{
    //从ini中读取配置路径
    QString path = pIni->value("/BG/Path").toString();

    //判断路径是否合法
    if(path.isEmpty()){
        return "";
    }

    return path;
}

//设置App目录
void ManageData::setAppPath(const QList<QString> &file_path)
{
    pIni->beginWriteArray("App");
    for(int i = 0;i < file_path.length();i++){
        pIni->setArrayIndex(i);
        pIni->setValue("Path", file_path[i]);
    }
    pIni->endArray();
}

//读取App目录
QList<QString> ManageData::getAppPath() const
{
    QList<QString> file_list;
    int count = pIni->beginReadArray("App");
    for(int i = 0;i < count;i++){
        pIni->setArrayIndex(i);
        file_list.append(pIni->value("Path").toString());
    }
    pIni->endArray();

    return file_list;
}

//Url
QString ManageData::getLoginUrl() const
{    
    QString url = pIni->value("/Url/Login").toString();
    if(url.isEmpty()){
        return "";
    }
    return url;
}
