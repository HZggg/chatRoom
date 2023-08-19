#ifndef LOGIN_H
#define LOGIN_H

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
    void on_btnLogin_clicked();

private:
    Ui::Login *ui;

    QPoint mOffset; //鼠标点击时，点击点与窗口左上角的偏移
};

#endif // LOGIN_H
