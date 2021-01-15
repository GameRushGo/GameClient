#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "managedialog.h"
#include "logindialog.h"
#include "checkprocess.h"
#include "cglobal.h"
#include "easylogging++.h"

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
    EEG_process = new QProcess();
    pointer_process = new QProcess();
    flop_process = new QProcess();
    voice_process = new QProcess();
    handEye_process = new QProcess();

    //timer
    timer = new QTimer(this);

    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(1);
    //绑定按钮点击事件，并启动不同进程
    //EEG
    connect(ui->EEGButton,&QPushButton::pressed,
            [=]()
            {                
                ui->stackedWidget->setCurrentIndex(2);
            }
    );

    //BG(大游戏) button
    connect(ui->BGButton,&QPushButton::pressed,
            [=]()
            {
                ui->stackedWidget->setCurrentIndex(1);
            }
    );

    //大游戏
    connect(ui->StartBGButton, &QPushButton::pressed,
            [=]()
            {
                STARTUPINFO si = {sizeof(si)};
                PROCESS_INFORMATION pi ;
                QString loginDataPath;
                QString currentDir;

                //获取存档目录
                loginDataPath = QDir::currentPath() + "/login.dat";
                currentDir = QDir::currentPath() + QString::fromLocal8Bit("/App/大游戏");

                wchar_t *wAppName = new wchar_t[BG_path.length() + 1];
                wchar_t *wCmdLine = new wchar_t[loginDataPath.length() + 1];
                wchar_t *wCurrentDir = new wchar_t[currentDir.length() + 1];

                //qstring -> wchar_t*
                BG_path.toWCharArray(wAppName);
                loginDataPath.toWCharArray(wCmdLine);
                currentDir.toWCharArray(wCurrentDir);
                wAppName[BG_path.length()] = '\0';
                wCmdLine[loginDataPath.length()] = '\0';
                wCurrentDir[currentDir.length()] = '\0';

                //创建游戏进程
                CreateProcessW(wAppName, NULL, NULL, NULL, FALSE, 0, NULL, wCurrentDir, &si, &pi);

                LOG(INFO) << wAppName;
                LOG(INFO) << wCmdLine;

                delete[] wAppName;
                delete[] wCmdLine;
                delete[] wCurrentDir;
            }
    );

    //EEG
    connect(ui->StartEEGButton, &QPushButton::pressed,
            [=]()
            {
                EEG_process->start(EEG_path);
            }
            );

    //AssistButton
    connect(ui->AssistButton, &QPushButton::pressed,
            [=]()
            {
                ui->stackedWidget->setCurrentIndex(0);
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
            }
    );

    //Voice
    connect(ui->VoiceButton,&QPushButton::pressed,
            [=]()
            {
                voice_process->start(voice_path);
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
                onSetInfo();
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

    //checkbox
    connect(ui->checkBox, &QPushButton::clicked, this, &MainWindow::onCheckBox);

    //timer
    connect(this->timer, &QTimer::timeout, this, &MainWindow::onTimer);

    //启动照片墙
    initGameLabel();
    initEEGLabel();

    //在窗体启动时检查是否存在未关闭的应用
    app_list = CheckProcess::getFilePath();
    CheckProcess::LockProcess(app_list);

    //首先启动登录窗口
    LoginDialog *login_dialog = new LoginDialog();
    login_dialog->exec();
    delete login_dialog;

    onSetInfo();
    //检查是否登录成功，若不成功将自动退出程序
    if(!isLogin) exit(0);
}


MainWindow::~MainWindow()
{
    //关闭进程
    EEG_process->close();
    handEye_process->close();
    voice_process->close();
    flop_process->close();
    pointer_process->close();

    //释放进程指针
    if(EEG_process) delete EEG_process;
    if(handEye_process) delete handEye_process;
    if(voice_process) delete voice_process;
    if(flop_process) delete flop_process;
    if(pointer_process) delete pointer_process;

    //检查是否存在未终止的应用并关停
    CheckProcess::LockProcess(app_list);

    //终止计时器
    timer->stop();

    delete file_watcher;
    delete m_data;
    delete timer;
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
        else if(path.contains(QString::fromLocal8Bit("记忆"))){
            flop_path = path;
        }
        else if(path.contains("FanFanLe6")){
            voice_path = path;
        }
        else if(path.contains(QString::fromLocal8Bit("手眼协调"))){
            handEye_path = path;
        }
        else
            qDebug() << "Illegal Path!";
    }
}

//checkBox
void MainWindow::onCheckBox()
{
    static int times = 0;
    //奇偶校验
    if(!(times++ & 1)){
        ui->checkBox->setStyleSheet(QString::fromLocal8Bit("image: url(:/Photo/MainWindow/选中.png);"));
        ui->showInfoLabel->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" color:#b9b9b9;\">显示敏感信息</span></p></body></html>"));

        //id
        if(user_info.getIdentity().isEmpty()){
            ui->IDLabel->setText(QString::fromLocal8Bit("测试ID"));
        }else{
            ui->IDLabel->setText(user_info.getIdentity());
        }

        //tel
        if(user_info.getTel().isEmpty()){
            ui->telLabel->setText(QString::fromLocal8Bit("测试电话"));
        }else{
            ui->telLabel->setText(user_info.getTel());
        }

    }else{
        ui->checkBox->setStyleSheet(QString::fromLocal8Bit("image: url(:/Photo/MainWindow/未选中.png);"));
        ui->showInfoLabel->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" color:#b9b9b9;\">隐藏敏感信息</span></p></body></html>"));
        ui->IDLabel->setText("******************");
        ui->telLabel->setText("***********");
    }
}

//CSGO图片墙
void MainWindow::initGameLabel()
{
    game_pix_list.append("image: url(:/Photo/Game/csgo.0.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.1.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.2.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.3.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.4.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.5.jpg);");
    game_pix_list.append("image: url(:/Photo/Game/csgo.6.jpg);");

    game_opacity = new QGraphicsOpacityEffect(this);
    ui->GameLabel->setGraphicsEffect(game_opacity);
    game_opacity->setOpacity(0);

    //设置渐变动画，持续800ms
    game_animation = new QPropertyAnimation(this->game_opacity, "opacity");
    game_animation->setDuration(800);
    game_animation->setStartValue(0);
    game_animation->setEndValue(1);
    game_animation->setEasingCurve(QEasingCurve::Linear);
    game_animation->start();

    timer->start(5000);
}

//EEG图片墙
void MainWindow::initEEGLabel()
{
    eeg_pix_list.append("image: url(:/Photo/EEG/eeg_0_m.jpg);");
    eeg_pix_list.append("image: url(:/Photo/EEG/eeg_1_m.jpg);");
    eeg_pix_list.append("image: url(:/Photo/EEG/eeg_2_m.jpg);");
    eeg_pix_list.append("image: url(:/Photo/EEG/eeg_3_m.jpg);");
    eeg_pix_list.append("image: url(:/Photo/EEG/eeg_4_m.jpg);");

    eeg_opacity = new QGraphicsOpacityEffect(this);
    ui->EEGLabel->setGraphicsEffect(eeg_opacity);
    eeg_opacity->setOpacity(0);

    //设置渐变动画
    eeg_animation = new QPropertyAnimation(this->eeg_opacity, "opacity");
    eeg_animation->setDuration(800);
    eeg_animation->setStartValue(0);
    eeg_animation->setEndValue(1);
    eeg_animation->setEasingCurve(QEasingCurve::Linear);
    eeg_animation->start();
}

//响应计时器，循环播放图片，GameLabel和EEGLabel共用一个timer
void MainWindow::onTimer()
{
    static int game_cnt = 0;
    static int eeg_cnt = 0;

    ui->GameLabel->setStyleSheet(game_pix_list.at(game_cnt++));
    game_animation->start();

    ui->EEGLabel->setStyleSheet(eeg_pix_list.at(eeg_cnt++));
    eeg_animation->start();

    if(game_cnt > 6){
        game_cnt = 0;
    }

    if(eeg_cnt > 4){
        eeg_cnt = 0;
    }
}

//读取json数据并显示
void MainWindow::onSetInfo()
{
    //使用"-"代表不存在的用户信息
    QString empty_str = QString::fromLocal8Bit("测试");
    //name
    if(user_info.getName().isEmpty()){
        ui->nameLabel->setText(empty_str);
    }else{
        ui->nameLabel->setText(user_info.getName());
    }

    //groupNum
    if(user_info.getGroupnum() == 0){
        ui->groupnumLabel->setText(empty_str);
    }else{
        ui->groupnumLabel->setText(QString::number(user_info.getGroupnum()));
    }
    //research
    if(user_info.getResearch() == 0){
        ui->researchLabel->setText(empty_str);
    }else{
        ui->researchLabel->setText(QString::number(user_info.getResearch()));
    }

    //department
    if(user_info.getDepartment().isEmpty()){
        ui->departmentLabel->setText(empty_str);
    }else{
        ui->departmentLabel->setText(user_info.getDepartment());
    }

    //grade
    if(user_info.getGrade().isEmpty()){
        ui->gradeLabel->setText(empty_str);
    }else{
        ui->gradeLabel->setText(user_info.getGrade());
    }

    //weight
    if(user_info.getWeight() == 0.0){
        ui->weightLabel->setText(empty_str);
    }else{
        ui->weightLabel->setText(QString::number(user_info.getWeight()));
    }

    //vision
    if(user_info.getVision().isEmpty()){
        ui->visionLabel->setText(empty_str);
    }else{
        ui->visionLabel->setText(user_info.getVision());
    }

    //account
    if(user_info.getAccount().isEmpty()){
        ui->accountLabel->setText(empty_str);
    }else{
        ui->accountLabel->setText(user_info.getAccount());
    }

}
