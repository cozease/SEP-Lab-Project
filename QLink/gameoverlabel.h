#pragma once

#ifndef GAMEOVERLABEL_H
#define GAMEOVERLABEL_H

#include <QWidget>
#include <QLabel>

class GameOverLabel : public QLabel
{
    Q_OBJECT

public:
    // explicit GameOverLabel(QWidget *parent = nullptr);
    GameOverLabel(QWidget *parent, const int &score1, const int &score2 = -1);
    // ~GameOverLabel();

    // 根据游戏得分显示文本
    const int score1;
    const int score2;

protected:
    void paintEvent(QPaintEvent *) override;

signals:
};

#endif // GAMEOVERLABEL_H
