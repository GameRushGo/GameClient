#include "mainwindow.h"
#include "easylogging++.h"

#include <QApplication>
#include <QDir>

INITIALIZE_EASYLOGGINGPP

/*
 *  数据管理：
 * 	1、8个程序的相对路径由配置文件管理；
    2、如果是管理人员登录平台，提供一个管理这些程序路径的界面；
    3、附件“小游戏”中有4个小游戏，通过主界面中的4个小游戏按钮能够分别启动其一；
    4、确保多个游戏程序中只启动一个实例；
    5、确保主程序退出时如果有启动的游戏程序，需要一并退出。

    配置文件格式设计如下：

    [Common]
    Path=data_path;
    [EEG]
    Path=EEG_path;
    [BG]
    Path=BigGame_path;
    [App]
    1\Path=test/app1.exe
    2\Path=test/app2.exe
    3\Path=test/app3.exe
    4\Path=test/app4.exe
    size=4

    接口请参考ManageData类.

 *  游戏路径管理：
    在正常情况下，浏览按钮被锁定，通过勾选checkbox复选框可以接触锁定；
    在点击“浏览”按钮后，会启动文件对话框，并将选择的路径显示在lineedit中；
    最后，析构函数会检查路径是否存在更新，若存在则把更新写入配置文件保存

*/

void init()
{
    //检查Log目录是否存在
    QDir *dir = new QDir("Log");
    if(!dir->exists()){
        dir->mkdir(QCoreApplication::applicationDirPath() + "/Log");
    }

    //初始化日志系统
    el::Configurations conf(QString(QCoreApplication::applicationDirPath() + "/logConf.conf").toStdString());
    el::Loggers::reconfigureAllLoggers(conf);

    LOG(INFO) << "***** App Start *****";

    if(dir){
        delete dir;
        dir = NULL;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //初始化Log日志系统
    init();    
    MainWindow w;
    w.show();
    return a.exec();
}
