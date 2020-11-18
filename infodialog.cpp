#include "infodialog.h"
#include "ui_infodialog.h"
#include "cglobal.h"

InfoDialog::InfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    //绑定确认按钮->关闭窗口
    connect(ui->pushButton, &QPushButton::pressed,
            [=]()
            {
                accept();
            }
            );
    //输出用户信息
    setInfo();
}

InfoDialog::~InfoDialog()
{    
    delete ui;
}


//显示用户信息
void InfoDialog::setInfo()
{
    QString empty_str = "-";
    //写入用户信息，如果为空，则使用"-"替换
    //groupnum
    if(user_info.getGroupnum() == 0){
        ui->groupnum_label->setText(empty_str);
    }else{
        ui->groupnum_label->setText(QString::number(user_info.getGroupnum()));
    }

    //research
    if(user_info.getResearch() == 0){
        ui->research_label->setText(empty_str);
    }else{
        ui->research_label->setText(QString::number(user_info.getResearch()));
    }

    //name
    if(user_info.getName().isEmpty()){
        ui->name_label->setText(empty_str);
    }else{
        ui->name_label->setText(user_info.getName());
    }

    //id
    if(user_info.getIdentity().isEmpty()){
        ui->id_label->setText(empty_str);
    }else{
        ui->id_label->setText(user_info.getIdentity());
    }

    //tel
    if(user_info.getTel().isEmpty()){
        ui->tel_label->setText(empty_str);
    }else{
        ui->tel_label->setText(user_info.getTel());
    }

    //department
    if(user_info.getDepartment().isEmpty()){
        ui->department_label->setText(empty_str);
    }else{
        ui->department_label->setText(user_info.getDepartment());
    }

    //grade
    if(user_info.getGrade().isEmpty()){
        ui->grade_label->setText(empty_str);
    }else{
        ui->grade_label->setText(user_info.getGrade());
    }

    //weight
    if(user_info.getWeight() == 0.0){
        ui->weight_label->setText(empty_str);
    }else{
        ui->weight_label->setText(QString::number(user_info.getWeight()));
    }

    //vision
    if(user_info.getVision().isEmpty()){
        ui->vision_label->setText(empty_str);
    }else{
        ui->vision_label->setText(user_info.getVision());
    }

    //account
    if(user_info.getAccount().isEmpty()){
        ui->account_label->setText(empty_str);
    }else{
        ui->account_label->setText(user_info.getAccount());
    }
}
