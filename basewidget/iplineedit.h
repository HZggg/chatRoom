#ifndef IPLINEEDIT_H
#define IPLINEEDIT_H
#include <QLineEdit>
#include <QWidget>
class IPLineEdit : public QLineEdit
{
public:
    IPLineEdit(QWidget *parent = nullptr);
    ~IPLineEdit();
    QString text();
protected:
    bool eventFilter(QObject* obj,QEvent *event);
private:
    QLineEdit *m_lineEdit[4]; //IP地址的4个输入位置  *.*.*.*
    int getIndex(QLineEdit *pLineEdit);//获取下标
};

#endif // IPLINEEDIT_H
