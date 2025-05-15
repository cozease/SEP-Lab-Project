#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QLabel>

class Player : public QLabel
{
    Q_OBJECT

public:
    // explicit Player(QWidget *parent = nullptr);
    // sex为false表示第一个玩家，为true表示第二个玩家
    Player(const bool &sex);
    // ~Player();

    // sexNum为0表示第一个玩家，为1表示第二个玩家
    const int sexNum;

protected:
    void paintEvent(QPaintEvent *) override;

signals:
};

#endif // PLAYER_H
