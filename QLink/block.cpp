#include "block.h"

#include <QDebug>
#include <QPainter>

// Block::Block(QWidget *parent)
//     : QWidget{parent}
// {}

Block::Block(const QString &type)
    : type(type)
    , isSelected(false)
    , isSelectedByAether(false)
    , isRemoved(false)
    , isHighlighted(false)
{
    setFixedSize(sideLen, sideLen);
}

// Block::~Block()
// {
//     QLabel::~QLabel();
// }

void Block::paintEvent(QPaintEvent *event)
{
    // 如果未消除，则绘制方块图案
    if (!isRemoved) {
        QPainter painter(this);
        painter.drawPixmap(0, 0, width(), height(), QPixmap(":/resources/" + type + ".png"));

        // 高亮时绘制蓝色边框
        if (isHighlighted) {
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(QPen(Qt::red, sideLen / 10));
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(0, 0, width(), height());
        }

        // 根据选中方块的角色绘制对应的边框颜色，优先级高于高亮边框
        if (isSelected) {
            painter.setPen(QPen(Qt::yellow, sideLen / 10));
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(0, 0, width(), height());
        }
        if(isSelectedByAether) {
            painter.setPen(QPen(Qt::blue, sideLen / 10));
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(0, 0, width(), height());
        }
    }
}

void Block::remove()
{
    isRemoved = true;
    emit removeSignal();
}
