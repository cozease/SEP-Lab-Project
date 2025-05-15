#pragma once

#ifndef BLOCK_H
#define BLOCK_H

#include <QWidget>
#include <QLabel>

const int sideLen = 50;  // 地图中每格的边长

// enum BlockType {Venti, Zhongli, RaidenShogun, Nahida, Furina};

class Block : public QLabel
{
    Q_OBJECT

public:
    // explicit Block(QWidget *parent = nullptr);
    Block(const QString &type);
    // ~Block();

    // 方块的种类
    QString type;

    // 方块的状态
    bool isSelected;  // 是否被荧选中
    bool isSelectedByAether;  // 是否被空选中
    bool isRemoved;
    bool isHighlighted;

    // 消除方块
    void remove();

protected:
    void paintEvent(QPaintEvent *) override;

signals:
    // 被移除时发出的信号
    void removeSignal();
};

#endif // BLOCK_H
