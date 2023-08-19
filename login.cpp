#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    //设置界面无边框
    setWindowFlag(Qt::FramelessWindowHint);

    //跳转设置
    connect(ui->btnSet,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });

    //退出
    connect(ui->btnCloseWin,&QPushButton::clicked,[=](){
        qApp->exit(0);
    });

    connect(ui->btnCloseWin2,&QPushButton::clicked,[=](){
        qApp->exit(0);
    });

}

Login::~Login()
{
    delete ui;
}
