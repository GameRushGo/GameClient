#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "managedialog.h"
#include "logindialog.h"
#include "checkprocess.h"
#include "cglobal.h"
#include <QProcess>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //初始化程序路径
    initPath();

    //启动文件监控
    file_watcher = new FileWatcher();

    //初始化进程
    BG_process = new QProcess();
    EEG_process = new QProcess();
    pointer_process = new QProcess();
    flop_process = new QProcess();
    voice_process = new QProcess();
    handEye_process = new QProcess();

    ui->setupUi(this);

    //隐藏第二菜单button
    ui->VoiceButton->setVisible(false);
    ui->HandEyeButton->setVisible(false);
    ui->FlopButton->setVisible(false);
    ui->PointerButton->setVisible(false);
    ui->BackButton->setVisible(false);

    //绑定按钮点击事件，并启动不同进程
    //EEG
    connect(ui->EEGButton,&QPushButton::pressed,
            [=]()
            {                
                EEG_process->start(EEG_path);
            }
    );

    //BG(大游戏)
    connect(ui->BGButton,&QPushButton::pressed,
            [=]()
            {
                BG_process->start(BG_path);
            }
    );

    //HandEye
    connect(ui->HandEyeButton,&QPushButton::pressed,
            [=]()
            {
                handEye_process->start(handEye_path);
            }
    );

    //Poninter
    connect(ui->PointerButton,&QPushButton::pressed,
            [=]()
            {
                pointer_process->start(pointer_path);
            }
    );

    //Flop
    connect(ui->FlopButton,&QPushButton::pressed,
            [=]()
            {
                flop_process->start(flop_path);
                qDebug() << flop_path;
            }
    );

    //Voice
    connect(ui->VoiceButton,&QPushButton::pressed,
            [=]()
            {
                voice_process->start(voice_path);
                qDebug() << voice_path;
            }
    );

    //辅助
    connect(ui->AssistButton, &QPushButton::pressed,
            [=]()
            {
                //显示第二菜单button
                ui->BackButton->setVisible(true);
                ui->VoiceButton->setVisible(true);
                ui->FlopButton->setVisible(true);
                ui->PointerButton->setVisible(true);
                ui->HandEyeButton->setVisible(true);
                //隐藏主菜单button
                ui->BGButton->setVisible(false);
                ui->ManageButton->setVisible(false);
                ui->EEGButton->setVisible(false);
                ui->LogoutButton->setVisible(false);
                ui->AssistButton->setVisible(false);
            }
            );

    //返回
    connect(ui->BackButton, &QPushButton::pressed,
            [=]()
            {
                //显示第二菜单button
                ui->BackButton->setVisible(false);
                ui->VoiceButton->setVisible(false);
                ui->FlopButton->setVisible(false);
                ui->PointerButton->setVisible(false);
                ui->HandEyeButton->setVisible(false);
                //隐藏主菜单button
                ui->BGButton->setVisible(true);
                ui->ManageButton->setVisible(true);
                ui->EEGButton->setVisible(true);
                ui->LogoutButton->setVisible(true);
                ui->AssistButton->setVisible(true);
            }
            );

    //数据管理窗口(manageDialog)
    connect(ui->ManageButton, &QPushButton::pressed,
            [=]()
            {
                //以模态方式启动管理窗口
                ManageDialog *manage_dialog = new ManageDialog();
                manage_dialog->exec();
                delete manage_dialog;

                //在打开管理页和可能会对数据更新，因此需重新初始化路径
                initPath();
            }
    );

    //注销窗口
    connect(ui->LogoutButton, &QPushButton::pressed,
            [=]()
            {
                /* 在点击注销按钮后，将强制退出所有app;
                 * 随后启动登录窗口并隐藏MainWindow
                 * 若登录成功，则重新显示MainWindow，否则将直接退出程序.
                */
                isLogin = false;
                CheckProcess::LockProcess(app_list);

                LoginDialog *login_dialog = new LoginDialog();
                this->setVisible(false);
                login_dialog->exec();
                delete login_dialog;

                if(!isLogin){
                    qDebug() << "登录失败.";
                    exit(0);
                }
                this->setVisible(true);
            }
            );

    //ftp测试窗口
    connect(ui->TestButton, &QPushButton::pressed,
            []()
            {
                //以模态方式启动调试ftp调试窗口
                FtpDialog *ftp_dialog = new FtpDialog();
                ftp_dialog->exec();
                delete ftp_dialog;
            }
            );

    //用户信息窗口
    connect(ui->InfoButton, &QPushButton::pressed,
            []()
            {
                //以模态方式启动info窗口
                InfoDialog *info_dialog = new InfoDialog();
                info_dialog->exec();
                delete info_dialog;
            }
            );

    //在窗体启动时检查是否存在未关闭的应用
    app_list = CheckProcess::getFilePath();
    CheckProcess::LockProcess(app_list);

    //首先启动登录窗口
    LoginDialog *login_dialog = new LoginDialog();
    login_dialog->exec();
    delete login_dialog;

    //检查是否登录成功，若不成功将自动退出程序
    if(!isLogin) exit(0);
}


MainWindow::~MainWindow()
{
    //关闭进程
    EEG_process->close();
    BG_process->close();
    handEye_process->close();
    voice_process->close();
    flop_process->close();
    pointer_process->close();

    //释放进程指针
    if(EEG_process) delete EEG_process;
    if(BG_process) delete BG_process;
    if(handEye_process) delete handEye_process;
    if(voice_process) delete voice_process;
    if(flop_process) delete flop_process;
    if(pointer_process) delete pointer_process;

    //检查是否存在未终止的应用并关停
    CheckProcess::LockProcess(app_list);

    delete file_watcher;
    delete m_data;
    delete ui;
}

//初始化程序路径
void MainWindow::initPath()
{
    m_data = new ManageData();
    this->EEG_path = m_data->getEEGPath();
    this->BG_path = m_data->getBGPath();

    foreach(QString path, m_data->getAppPath()){
        if(path.contains("StickPinGame")){
            pointer_path = path;
        }
        else if(path.contains("记忆")){
            flop_path = path;
        }
        else if(path.contains("FanFanLe6")){
            voice_path = path;
        }
        else if(path.contains("手眼协调")){
            handEye_path = path;
        }
        else
            qDebug() << "Illegal Path!";
    }
}
