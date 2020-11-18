QT       += core gui
QT       += network    #web服务
QT       += winextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cglobal.cpp \
    checkprocess.cpp \
    filewatcher.cpp \
    ftpclient.cpp \
    ftpdialog.cpp \
    infodialog.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    managedata.cpp \
    managedialog.cpp \
    userinfo.cpp

HEADERS += \
    cglobal.h \
    checkprocess.h \
    filewatcher.h \
    ftpclient.h \
    ftpdialog.h \
    infodialog.h \
    logindialog.h \
    mainwindow.h \
    managedata.h \
    managedialog.h \
    structs.h \
    userinfo.h

FORMS += \
    ftpdialog.ui \
    infodialog.ui \
    logindialog.ui \
    mainwindow.ui \
    managedialog.ui \
    testdialog.ui

TRANSLATIONS += \
    GameClient_zh_CN.ts

#import QFtp
CONFIG(debug, debug|release) {
    LIBS += -lQt5Ftpd
} else {
    LIBS += -lQt5Ftp
}
LIBS += -LD:/QT5.14/5.14.2/msvc2015_64/lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

DISTFILES += \
    Log/QFtp模块编译与修改记录.txt \
    Photo/头像.jpg \
    Photo/头像.png
