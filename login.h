#ifndef LOGIN_H
#define LOGIN_H
#include "clientsocket.h"

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

protected:

    //窗口移动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void on_btnLogin_clicked(); //登录

    void onSignalMessage(const quint8 &type, const QJsonValue &dataVal);
    void onSignalStatus(const quint8 &state);

private:
    Ui::Login *ui;

    QPoint mOffset; //鼠标点击时，点击点与窗口左上角的偏移

    ClientSocket *m_tcpSocket;
};

#endif // LOGIN_H
