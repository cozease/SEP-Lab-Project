#include "player.h"

#include <QPainter>

// Player::Player(QWidget *parent)
//     : QWidget{parent}
// {}

Player::Player(const bool &sex)
    : sexNum(sex ? 1 : 0)
{
    setFixedSize(50, 50);
}

// Player::~Player()
// {
//     QLabel::~QLabel();
// }

void Player::paintEvent(QPaintEvent *)
{
    // 绘制角色
    QPainter painter(this);
    QPixmap traveler(QString(":/resources/Traveler_%1.png").arg(sexNum));
    painter.drawPixmap(0, 0, width(), height(), traveler);

    // 绘制边框
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0.0, Qt::red);
    gradient.setColorAt(0.5, Qt::green);
    gradient.setColorAt(1.0, Qt::blue);
    painter.setPen(QPen(gradient, 5));
    painter.drawRect(0, 0, width(), height());
}
