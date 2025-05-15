#pragma once

#ifndef DOUBLESCENE_H
#define DOUBLESCENE_H

#include "playscene.h"
#include "player.h"

#include <QWidget>

class DoubleScene : public PlayScene
{
    Q_OBJECT

public:
    // explicit DoubleScene(QWidget *parent = nullptr);
    DoubleScene(const int &time, const int &M, const int &N, const bool &isNewGame = true);

    // 玩家
    Player *Lumine;
    Player *Aether;
    QPoint posOfLumine;
    QPoint posOfAether;
    void playerInit() override;
    void playerMove(QKeyEvent *&event);

    // 记录被选中的方块
    Block *selectedByLumine;
    Block *selectedByAether;

    // 玩家碰到方块时的处理
    void handle(Block *&block, bool isLumine = true) override;

    // 倒计时的封装
    void timerInit() override;

    // 暂停按钮的封装
    void pauseInit() override;

    // 保存按钮的封装
    void saveInit() override;

    // 计分板的封装
    int scoreOfLumine;
    int scoreOfAether;
    QLabel *scoreboardOfLumine;
    QLabel *scoreboardOfAether;
    void drawScoreboard() override;

    // 游戏结束
    void gameOver() override;

    // 道具的封装
    void propInit() override;
    void propTrigger() override;

    // 冰冻道具
    QLabel *freeze;
    void propFreeze(const QPoint &propPos);
    bool isLumineFrozen;
    bool isAetherFrozen;

    // 颠倒道具
    QLabel *dizzy;
    void propDizzy(const QPoint &propPos);
    bool isLumineDizzy;
    bool isAetherDizzy;

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
};

#endif // DOUBLESCENE_H
