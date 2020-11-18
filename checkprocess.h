#ifndef CHECKTHREAD_H
#define CHECKTHREAD_H

#include <QList>
#include <QSettings>
#include <QDebug>
#include <QSharedMemory>
#include <Windows.h>
#include <tchar.h>
#include <Tlhelp32.h>
#include <QVector>

/*
 * 本类用于防止应用多开，包含二个静态函数，功能如下：
 * getFilePath() 读取ini文件并获取待检测的项目名（ini中存储的为程序路径，因此需要对路径进行分割，提取程序名）
 * LockProcess() 监测并杀死目标进程，用于程序启动时和程序结束时
*/
class CheckProcess: public QObject
{
    Q_OBJECT
public:
    static QList<QString> getFilePath();                     //通过读取ini文件获取程序启动路径
    static void LockProcess(const QList<QString> &list);           //扫描并防止进程多开
};

#endif // CHECKTHREAD_H
