#ifndef MANAGEDATA_H
#define MANAGEDATA_H

#include <QString>
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QObject>
#include <QtCore>

class ManageData
{
public:
    ManageData();
    ~ManageData();
    //管理数据文件路径
    void setDataPath(const QString &value);
    QString getDataPath() const;

    //管理EEG脑电程序路径
    void setEEGPath(const QString &value);
    QString getEEGPath() const;

    //管理BG大游戏路径
    void setBGPath(const QString &value);
    QString getBGPath() const;

    //管理App游戏程序路径
    void setAppPath(const QList<QString> &file_path);
    QList<QString> getAppPath() const;

    //管理url
    void setFtpUrl(const QString& url);
    QString getFtpUrl();

    void setLoginUrl(const QString& url);
    QString getLoginUrl() const;

    //ToSring函数，返回所有数据[保留]
    QString toString();

private:
    QSettings *pIni;
    QString data_path;
    QString EGG_path;
    QString BG_path;
    QList<QString> app_path;
};

#endif // MANAGEDATA_H

