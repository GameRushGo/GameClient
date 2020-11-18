#ifndef MANAGEDIALOG_H
#define MANAGEDIALOG_H

#include <QDialog>
#include "managedata.h"

namespace Ui {
class ManageDialog;
}

class ManageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManageDialog(QWidget *parent = nullptr);
    ~ManageDialog();
    void getPath();
    QString setPath(const QString &dir, const QString filter = "*.exe");

private:
    Ui::ManageDialog *ui;
    ManageData *m_data;         //数据管理对象
    QString EGG_path;
    QString BG_path;
    QString pointer_path;
    QString flop_path;
    QString voice_path;
    QString handEye_path;

    bool isModify = false;      //标注配置文件是否需要更新
    QList<QString> app_list;
    QList<QString> file_path;
};

#endif // MANAGEDIALOG_H
