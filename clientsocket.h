#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

class ClientSocket : public QObject
{
    Q_OBJECT
public:
    explicit ClientSocket(QObject *parent = 0);
    ~ClientSocket();


    int GetUserId() const; //获取当前用户id
    void SetUserId(const int &id); //设置当前socket的用户id

    void CheckConnected(); //检测是否连接服务器，没有连接，调用这个函数会连接
    void ColseConnected(); //主动断开连接

    // 连接服务器
    void ConnectToHost(const QString &host, const int &port);
    void ConnectToHost(const QHostAddress &host, const int &port);

signals:
    //这两个信号可能在以下情况下被触发：
    //1. signalMessage信号：当客户端收到来自服务器的消息时，会触发这个信号。
    //                     其中，type参数表示消息类型，dataVal参数表示消息的数据内容。
    //2. signalStatus信号：当客户端的连接状态发生变化时，会触发这个信号。
    //                    其中，state参数表示连接状态，可能的取值包括Connected（已连接）、Disconnected（已断开连接）等。
    //                    当客户端成功连接到服务器或者连接断开时，都会触发这个信号。
    void signalMessage(const quint8 &type, const QJsonValue &dataVal);
    void signalStatus(const quint8 &state);

public slots:
    // socket消息发送封装
    void SltSendMessage(const quint8 &type, const QJsonValue &dataVal);

private slots:
    // 与服务器断开链接
    void SltDisconnected();
    // 连接上服务器
    void SltConnected();
    // tcp消息处理 服务器消息处理，接收服务器返回的数据，也是json格式
    void SltReadyRead();

private:
    // tcpsocket
    QTcpSocket *m_tcpSocket;
    int m_nId; //用户id
    void ParseLogin(const QJsonValue &dataVal); //登录

};

#endif // CLIENTSOCKET_H
