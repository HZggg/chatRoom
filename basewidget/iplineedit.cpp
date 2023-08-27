#include "iplineedit.h"
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QLabel>
#include <QRegExp> //正则表达式
#include <QRegExpValidator>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>


IPLineEdit::IPLineEdit(QWidget *parent)
        :QLineEdit(parent)
{

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setContentsMargins(1,2,1,2);

    //正则
    QRegExp regExp("(25[0-5])|(2[0-4][0-9])|(1[0-9]{2})|[1-9][0-9]");//IP设置 最大255 、249、199

    QLabel * labelDot[3];//IP之间的点
    for (size_t i = 0; i<4; ++i)
    {
        m_lineEdit[i] = new QLineEdit(this);

        m_lineEdit[i]->setFrame(false);//去除边框
        m_lineEdit[i] ->setMaxLength(3);//IP最多3位 255
        m_lineEdit[i]->setAlignment(Qt::AlignCenter);//居中
        m_lineEdit[i]->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);//保存小部件的默认布局

        m_lineEdit[i]->setValidator(new QRegExpValidator(regExp,this)); //验证规则，判断输入是否正确

        //安装事件过滤器，实现按键可以左右移动光标到不同的输入框
        //过滤子控件事件，截获控件按键，鼠标事件
        m_lineEdit[i]->installEventFilter(this);

        hboxLayout->addWidget(m_lineEdit[i]);

        if(i<3)
        {
            labelDot[i] = new QLabel(this);
            labelDot[i]->setText(".");
            labelDot[i]->setFixedWidth(2);
            hboxLayout->addWidget(labelDot[i]);
        }
    }

    this->setReadOnly(true); //外面的LineEdit不能输入东西
    m_lineEdit[0]->setFocus();
    m_lineEdit[0]->selectAll();
}

//重写事件 过滤子控件事件，截获控件按键，鼠标事件
bool IPLineEdit::eventFilter(QObject* obj,QEvent *event)
{
    if(children().contains(obj) && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        QLineEdit *pCurrentEdit = qobject_cast<QLineEdit*>(obj); //当前的输入框，有4个

        //重写按键事件，只有下面的按键才有效
        switch (keyEvent->key()) {
            case Qt::Key_0:
            case Qt::Key_1:
            case Qt::Key_2:
            case Qt::Key_3:
            case Qt::Key_4:
            case Qt::Key_5:
            case Qt::Key_6:
            case Qt::Key_7:
            case Qt::Key_8:
            case Qt::Key_9: //这里是按键只有0-9才能按下，其他无效
            {
                QString strText = pCurrentEdit->text();

                //就是当你输入小于等于3位数，但是乘以10之后能大于255就跳到下一个输入框，
                //前面正则说明了，第一位最起码为1，所以输完3位最少为100，不一定*10，只要>255就行
                if (strText.length() <=3 && strText.toInt()*10 > 255)
                {
                    int index = getIndex(pCurrentEdit); //获取当前输入框的下标，第几个输入框

                    if (index !=-1 && index !=3) //不是最后一个输入框
                    {
                        m_lineEdit[index+1]->setFocus();
                        m_lineEdit[index+1]->selectAll();
                    }
                }
                return QLineEdit::eventFilter(obj, event);//其他事件默认
            }
            break;

            case Qt::Key_Left:
            {
                //cursorPosition:此属性保存此行编辑的当前光标位置。设置光标位置将在适当时重新绘制。默认情况下，此属性的值为0。
                if (!pCurrentEdit->cursorPosition()) // 默认值为0，然后可以移动，不好，当光标不在文本框的开头位置时， pCurrentEdit->cursorPosition()  的返回值就不会是0
                {
                    int index = getIndex(pCurrentEdit);
                    if (index != -1 && index !=0)
                    {
                        m_lineEdit[index-1]->setFocus();
                        int length = m_lineEdit[index-1]->text().length();
                        m_lineEdit[index-1]->setCursorPosition(length?length:0);
                    }
                }

                return QLineEdit::eventFilter(obj, event);
            }
            break;

            case Qt::Key_Right:
            {

                if (!pCurrentEdit->cursorPosition())
                {
                    int index = getIndex(pCurrentEdit);
                    if (index != -1 && index !=3)
                    {
                        m_lineEdit[index+1]->setFocus();
                        int length = m_lineEdit[index+1]->text().length();
                        m_lineEdit[index+1]->setCursorPosition(length?length:0);
                    }
                }

                return QLineEdit::eventFilter(obj, event);
            }
            break;

            case Qt::Key_Backspace: //后退键
            {
                QString strText = pCurrentEdit->text();
                if (strText.isEmpty())
                {
                    int index = getIndex(pCurrentEdit);
                    if (index != -1 && index !=0)
                    {
                        m_lineEdit[index-1]->setFocus();
                        int length = m_lineEdit[index-1]->text().length();
                        m_lineEdit[index-1]->setCursorPosition(length?length:0);
                    }
                }

                return QLineEdit::eventFilter(obj, event);
            }
            break;
            case Qt::Key_Period: //就是一个点号(.),就是输入点的话会自动跳到下一个输入框
            {
                int index = getIndex(pCurrentEdit);
                if (index != -1 && index !=3)
                {
                    m_lineEdit[index+1]->setFocus();
                    m_lineEdit[index+1]->selectAll();
                }

                return QLineEdit::eventFilter(obj, event);
            }
            break;
        }
    }
}

int IPLineEdit::getIndex(QLineEdit *pLineEdit)
{
    int index = -1;

    for (int i=0;i<4;i++)
    {
        if (pLineEdit == m_lineEdit[i])
        {
            index = i;
        }
    }
    return index;
}

QString IPLineEdit::text()
{
    //获取IP地址
    //192.168.1.1
    QString ip;
    for (int i=0;i<4;i++) {
        ip.append(m_lineEdit[i]->text());
        if (i != 3) {
            ip.append(".");
        }
    }
    return ip;
}

IPLineEdit::~IPLineEdit()
{

}
