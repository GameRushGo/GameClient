#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QDir>
#include <QGraphicsOpacityEffect>

#include "managedata.h"
#include "checkprocess.h"
#include "ftpclient.h"
#include "ftpdialog.h"
#include "userinfo.h"
#include "infodialog.h"
#include "filewatcher.h"
#include "cglobal.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initPath();
    void initGameLabel();
    void initEEGLabel();
    void onCheckBox();
    void onTimer();
    void onFinished();
    void onSetInfo();

private:
    Ui::MainWindow *ui;

    //配置文件管理类
    ManageData *m_data;

    //程序路径
    QString EEG_path;
    QString BG_path;
    QString pointer_path;
    QString flop_path;
    QString voice_path;
    QString handEye_path;
    QList<QString> app_list;

    //背景图路径
    QStringList game_pix_list;
    QStringList eeg_pix_list;

    //程序进程
    QProcess *EEG_process;
    QProcess *BG_process;
    QProcess *pointer_process;
    QProcess *flop_process;
    QProcess *voice_process;
    QProcess *handEye_process;

    //文件监控
    FileWatcher *file_watcher;

    QTimer *timer;
    QPropertyAnimation *game_animation;
    QGraphicsOpacityEffect  *game_opacity;
    QPropertyAnimation *eeg_animation;
    QGraphicsOpacityEffect *eeg_opacity;
};
#endif // MAINWINDOW_H
