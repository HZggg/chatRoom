#include "login.h"
#include "mainwindow.h"
#include "ui_login.h"
#include "unit.h"


#include <QJsonObject>

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


    m_tcpSocket = new ClientSocket;

    connect(m_tcpSocket, &ClientSocket::signalMessage, this, &Login::onSignalMessage);//
    connect(m_tcpSocket, &ClientSocket::signalStatus, this, &Login::onSignalStatus);//signalStatus服务端返回数据就会触发，然后调用函数

    //检查是否连接到服务器，没连接的话就连接
    m_tcpSocket->CheckConnected();
}

Login::~Login()
{
    delete ui;
}

//窗口移动
void Login::mousePressEvent(QMouseEvent *event)
{
    mOffset = mOffset = event->globalPos() - this->pos();
}

void Login::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

//登录跳转
void Login::on_btnLogin_clicked()
{
    QString username = ui->lineEditAccounts->text();
    QString passwd = ui->lineEditPassword->text();

    // 构建 Json 对象
    QJsonObject json;
    json.insert("name", username);
    json.insert("passwd", passwd);

    //m_tcpSocket->write: "{\"data\":{\"name\":\"hang\",\"passwd\":\"hang\"},\"from\":-1,\"type\":17}"
    m_tcpSocket->SltSendMessage(login, json); //发送给服务端
}

void Login::onSignalMessage(const quint8 &type, const QJsonValue &dataVal)
{

}

void Login::onSignalStatus(const quint8 &state) //state从ClientSocket::ParseLogin中的emit signalStatus(LoginRepeat)等传过来的
{
    switch (state)
    {
        case ConnectedHost: //用户登录成功
            ui->lblClientTitle->setText("已连接服务器");
        break;

        case LoginSuccess: //用户登录成功
        {
            qDebug() << "用户登录成功";

            //登录按钮
            //MainWindow 显示聊天界面
            MainWindow *mainWindow = new MainWindow;
            mainWindow->show();

            //登录窗口隐藏
            this->hide();
        }
        break;

        case LoginPasswdError: //用户未注册
        qDebug() << "用户未注册";
        break;

        case LoginRepeat: //用户已在线
        qDebug() << "用户已在线";
        break;
    }
}
