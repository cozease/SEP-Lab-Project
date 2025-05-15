#include "mypushbutton.h"

#include <QPainter>

// MyPushButton::MyPushButton(QWidget *parent)
//     : QWidget{parent}
// {}

MyPushButton::MyPushButton(QWidget *parent, const QString &text)
    : isMouseIn(false), isAble(true)
{
    setParent(parent);
    setFixedSize(240, 100);
    setFocusPolicy(Qt::NoFocus);  // 禁用默认焦点，防止按下键盘某些按键时引发意外操作（我弟玩的时候发现的qwq

    // 设置按钮上的文本
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);  // 居中显示
    label->setFont(QFont("华文楷体", 30));  // 字体和字号
    label->setAttribute(Qt::WA_TransparentForMouseEvents);  // 鼠标事件穿透，使按钮能正常点击
    label->move(0, 0);
    label->setFixedSize(width(), height());
    label->setText(text);
}

// MyPushButton::~MyPushButton()
// {
//     delete label;
//     QPushButton::~QPushButton();
// }

void MyPushButton::paintEvent(QPaintEvent *)
{
    // 绘制按钮
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(0.5);
    painter.setPen(Qt::NoPen);

    // 根据鼠标是否进入显示按钮颜色
    if (isMouseIn) {
        painter.setBrush(QColor(255, 218, 130));
    } else {
        painter.setBrush(QColor(133, 215, 255));
    }

    painter.drawRoundedRect(0, 0, width(), height(), 10, 10);
}

void MyPushButton::enterEvent(QEnterEvent *event)
{
    if (isAble) {
        isMouseIn = true;
        update();
    }
}

void MyPushButton::leaveEvent(QEvent *)
{
    if (isAble) {
        isMouseIn = false;
        update();
    }
}

void MyPushButton::resetText(const QString &text)
{
    label->setText(text);
}

void MyPushButton::disable()
{
    // 不响应鼠标点击
    setEnabled(false);

    // 不响应鼠标进入或离开
    isAble = false;
    isMouseIn = false;
    update();
}
