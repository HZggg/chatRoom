#include "clientsocket.h"
#include "unit.h"

#include <QFile>
#include <QDebug>
#include <QHostAddress>
#include <QDataStream>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>

#define SERVER_IP "192.168.56.1"
#define SERVER_PORT 60100

ClientSocket::ClientSocket(QObject *parent) :
    QObject(parent)
{
    m_nId = -1; //默认未登录状态

    m_tcpSocket = new QTcpSocket(this);

    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(SltReadyRead()));
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(SltConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(SltDisconnected()));
}

ClientSocket::~ClientSocket()
{
}

//获取当前用户id
int ClientSocket::GetUserId() const
{
    return  m_nId;
}

//设置当前socket的用户id
void ClientSocket::SetUserId(const int &id)
{
    m_nId = id;
}

//检测是否连接服务器，没有连接，调用这个函数会连接
void ClientSocket::CheckConnected()
{
    //没连接服务器 就连接
    if (m_tcpSocket->state() != QTcpSocket::ConnectedState)
    {
        m_tcpSocket->connectToHost(QString(SERVER_IP), SERVER_PORT);

    }
}

//主动断开连接
void ClientSocket::ColseConnected()
{
    if (m_tcpSocket->isOpen()) //检查TCP连接是否已经建立并且处于可用状态
    {
        m_tcpSocket->abort();//中止（或取消）当前的TCP连接。想要中止一个正在进行的TCP连接时，使用函数abort()来实现
    }
}

//连接接服务器接口
//接受一个QString类型的host和一个整数类型的port作为参数。
//这意味着我们可以传递一个字符串形式的主机地址和一个整数形式的端口号来连接服务器。
void ClientSocket::ConnectToHost(const QString &host, const int &port)
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
    }
    m_tcpSocket->connectToHost(host, port);
}

//连接接服务器接口
//接受一个QHostAddress类型的host和一个整数类型的port作为参数。
//QHostAddress是一个用于表示主机地址的类，它可以接受不同的地址格式，如IPv4、IPv6等。
//这意味着我们可以直接传递一个QHostAddress类型的对象来连接服务器。
void ClientSocket::ConnectToHost(const QHostAddress &host, const int &port)
{
    if (m_tcpSocket->isOpen())
    {
        m_tcpSocket->abort();
    }
    m_tcpSocket->connectToHost(host, port);
}

//未连接
void ClientSocket::SltDisconnected()
{
    qDebug() << "has disconnecetd";
    m_tcpSocket->abort();
}

//检测是否连接
void ClientSocket::SltConnected()
{
    qDebug() << "has connecetd";
    emit signalStatus(ConnectedHost); //signalStatus是一个信号的名称，自定义信号 会触发signalStatus(const quint8 &state)信号
                                      //表示连接状态已经改变为ConnectedHost（已连接到主机）。
                                      //通过发射这个信号，我们可以让其他部分的代码知道连接状态的改变，并执行相应的操作。
}


//tcp socket消息管理  将客户端的消息传到服务端
void ClientSocket::SltSendMessage(const quint8 &type, const QJsonValue &dataVal)
{
    // 连接服务器
    if (!m_tcpSocket->isOpen()) {
        m_tcpSocket->connectToHost(QString(SERVER_IP), SERVER_PORT);
        m_tcpSocket->waitForConnected(1000); //等待1000毫秒
    }
    // 超时1s后还是连接不上，直接返回
    if (!m_tcpSocket->isOpen()) {
        return;
    }

    // 构建 Json 对象，接收客服端发送的消息,这样的格式，登录如下
    /*
        {
            "data": {
                "name": "vico",
                "passwd": "vico"
            },
            "from": -1,
            "type": 17
        }
    */

    QJsonObject json;
    json.insert("type", type); //消息功能，查看枚举值，可知道表示注册，登录，重复登录，注销等等
    json.insert("from", m_nId); //发送用户的id
    json.insert("data", dataVal); //数据,json格式，包括了用户名跟密码

    // 构建 Json 文档
    QJsonDocument document;
    document.setObject(json);

    //m_tcpSocket->write: "{\"data\":{\"name\":\"hang\",\"passwd\":\"hang\"},\"from\":-1,\"type\":17}"
    qDebug() << "m_tcpSocket->write:" << document.toJson(QJsonDocument::Compact);
    //QJsonDocument::Compact是Qt中的一个函数，用于将JSON文档进行压缩。
    //它的作用是将JSON文档中的空白字符（如空格、制表符、换行符等）去除，从而减小文档的大小。
    //使用QJsonDocument::Compact函数可以将一个QJsonDocument对象进行压缩，返回一个压缩后的JSON文档。
    //这个函数通常在需要将JSON数据进行传输或存储时使用，可以减少数据的传输量和存储空间。

    m_tcpSocket->write(document.toJson(QJsonDocument::Compact)); //write 发送给服务端
}


//服务器消息处理，接收服务器返回的数据，也是json格式
void ClientSocket::SltReadyRead()
{
    //服务器返回数据，下面是登录数据 id=-1: 用户未注册 id=-2: 用户已在线 id>0: 登录成功, 返回用户 id
    //                           code=-2 用户已在线 code=-1 用户未注册 code=0 更新在线信息
    /*
            {
            "data": {
                "code": 0,
                "head": "3_head_64.png",
                "id": 3,
                "msg": "ok"
            },
            "from": 3,
            "type": 17
            }
    */
    //读取socket数据
    QByteArray byRead = m_tcpSocket->readAll();

    //转换为JSON文档
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(byRead, &jsonError);
    //解析未发生错误
    if (!document.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        //json文档对象
        if (document.isObject())
        {
            //转化为对象
            QJsonObject jsonObj = document.object();
            QJsonValue dataVal = jsonObj.value("data");
            int nFrom = jsonObj.value("from").toInt();
            int nType = jsonObj.value("type").toInt();

            switch (nType) //判断消息类型，是注册还是登录或者其他等等
            {
                case Register:

                break;

                case login:
                    ParseLogin(dataVal);
                break;
            }
        }
    }
}


// 登录
void ClientSocket::ParseLogin(const QJsonValue &dataVal)
{
    QJsonObject dataObj = dataVal.toObject();
    int id = dataObj.value("id").toInt();
    int code = dataObj.value("code").toInt();
    QString msg = dataObj.value("msg").toString();

    if (code == -2)
    {
        qDebug() << "用户已在线";
        emit signalStatus(LoginRepeat); //LoginRepeat是个枚举值 自定义信号 会触发signalMessage(const quint8 &type, const QJsonValue &dataVal)
        m_nId = id;
    }
    else if (code == -1)
    {
        qDebug() << "用户未注册";
        emit signalStatus(LoginPasswdError);
    }
    else if (code == 0 && msg == "ok")
    {
        qDebug() << "登录成功";
        emit signalStatus(LoginSuccess);
    }
}
