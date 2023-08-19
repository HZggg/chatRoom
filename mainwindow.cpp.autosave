#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置界面无边框
    setWindowFlag(Qt::FramelessWindowHint);

    //退出
    connect(ui->btnCloseWin,&QPushButton::clicked,[=](){
        qApp->exit(0);
    });
}

//窗口移动
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

MainWindow::~MainWindow()
{
    delete ui;
}
