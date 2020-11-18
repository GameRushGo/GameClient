#include "checkprocess.h"
#include "managedata.h"

//获取目录
QList<QString> CheckProcess::getFilePath()
{
    QList<QString> file_list;
    ManageData *data = new ManageData();
    file_list = data->getAppPath();

    //分离文件路径，只保留文件名
    for(int i = 0;i < file_list.length(); i++){
        QFileInfo appInfo(file_list[i]);
        file_list[i] = appInfo.fileName();
        qDebug() << file_list[i];
    }
    delete data;
    return file_list;
}

//防止进程多开
void CheckProcess::LockProcess(const QList<QString> &file_list)
{
    //创建枚举结构体
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    int i = 0;

    //查询第一个进程
    BOOL bMore = Process32First(hProcessSnap,&pe32);
    while(bMore){
        //检查列表中是否包含目标exeFile
        i = file_list.indexOf(QString::fromWCharArray(pe32.szExeFile));
        if(i != -1){
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if(hProcess){
                qDebug() << "Kill process: " << pe32.th32ProcessID;
                TerminateProcess(hProcess,4);
            }
            CloseHandle(hProcess);
        }

        //查询下一个进程
        bMore = Process32Next(hProcessSnap,&pe32);
    }

    //内存回收
    CloseHandle(hProcessSnap);
}
