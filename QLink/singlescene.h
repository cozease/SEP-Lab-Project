#pragma once

#ifndef SINGLESCENE_H
#define SINGLESCENE_H

#include "playscene.h"
#include "player.h"

#include <QWidget>

class SingleScene : public PlayScene
{
    Q_OBJECT

public:
    // explicit SingleScene(QWidget *parent = nullptr);
    SingleScene(const int &time, const int &M, const int &N, const bool &isNewGame = true);
    // ~SingleScene();

    // 玩家的封装
    Player *player;
    QPoint pos;  // 以游戏地图左上角为(0,0)的坐标下玩家的位置
    void playerInit() override;
    void playerMove(QKeyEvent *&event);

    // 记录被选中的方块
    Block *selected;

    // 实例化玩家碰到方块时的处理
    void handle(Block *&block, bool isLumine = true) override;

    // 倒计时的封装
    void timerInit() override;

    // 暂停按钮的封装
    void pauseInit() override;

    // 保存按钮的封装
    void saveInit() override;

    // 计分板的封装
    int score;
    QLabel *scoreboard;
    void drawScoreboard() override;

    // 游戏结束
    void gameOver() override;

    // 道具的封装
    void propInit() override;
    void propTrigger() override;

    // 鼠标道具
    QLabel *flash;
    void propFlash(const QPoint &propPos);
    bool isFlashing;
    int flashTime;
    QTimer *flashTimer;
    void runFlash();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
};

#endif // SINGLESCENE_H
