#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>

class UserInfo
{
public:
    UserInfo();

    //set & get
    void setGroupnum(const int groupnum){this->groupnum = groupnum;}
    int getGroupnum() const {return groupnum;}

    void setResearch(const int research){this->research = research;}
    int getResearch() const {return research;}

    void setName(const QString &name){this->name = name;}
    QString getName() const {return name;}

    void setIdentity(const QString &identity){this->identity = identity;}
    QString getIdentity() const {return identity;}

    void setTel(const QString &tel){this->tel = tel;}
    QString getTel() const {return tel;}

    void setDepartment(const QString &department){this->department = department;}
    QString getDepartment() const {return department;}

    void setGrade(const QString &grade){this->grade = grade;}
    QString getGrade() const {return grade;}

    void setWeight(const float weight){this->weight = weight;}
    float getWeight() const {return weight;}

    void setVision(const QString &vision){this->vision = vision;}
    QString getVision() const {return vision;}

    void setAccount(const QString &account){this->account = account;}
    QString getAccount() const {return account;}

private:
    int groupnum;
    int research;
    QString name;
    QString identity;
    QString tel;
    QString department;
    QString grade;
    float weight;
    QString vision;
    QString account;
};

#endif // USERINFO_H
